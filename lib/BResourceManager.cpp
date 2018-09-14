#include "GameEngine.h"

// TODO: should application incbin the resources binary?

#ifdef __XTENSA__ // ODROID GO TARGET
// TODO: incbin or whatever for odroid go (target)
extern const TUint8  reesources_bin_start[] asm("_binary_resources_bin_start");
#else // LINUX OR MAC TARGET

#include <stdio.h>

#ifdef __linux__

// LINUX TARGET
asm(
"   .section .rodata\n"
"   .global binary_Resources_bin_start\n"
"   .global binary_Resources_bin_end\n"
"   .balign 16\n"
"binary_Resources_bin_start:\n"
"   .incbin \"Resources.bin\"\n"
"binary_Resources_bin_end:\n"
"   .byte 0\n"
"   .align 4\n"
);
//extern TUint8    binary_Resources_bin_end[];\n"
extern __attribute((aligned(16))) TUint8 binary_Resources_bin_start[];
//extern TUint8    binary_Resources_bin_start[];
//extern TUint8    binary_Resources_bin_end[];
#else

// MAC TARGET
asm(
"   .section .rodata,.rodata\n"
  "   .global _binary_Resources_bin_start\n"
  "   .global _binary_Resources_bin_end\n"
  "   .align 4\n"
  "_binary_Resources_bin_start:\n"
  "   .incbin \"Resources.bin\"\n"
  "_binary_Resources_bin_end:\n"
  "   .byte 0\n"
  "   .align 4\n"
);
extern "C" {
extern TUint8 binary_Resources_bin_start[];
extern TUint8 binary_Resources_bin_end[];
//extern TUint8 binary_Resources_bin_start[];
}

#endif  // MAC TARGET

BResourceManager resourceManager(binary_Resources_bin_start);

#endif  // LINUX OR MAC TARGET

struct BitmapSlot {
  BitmapSlot(TInt16 aResourceId, TInt16 aImageType, BBitmap *aBitmap, TBool aCached = EFalse) {
    mResourceId = aResourceId;
    mImageType = aImageType;
    mBitmap = aBitmap;
    mCached = aCached;
  }
  TInt16  mResourceId;
  TInt16  mImageType;
  BBitmap *mBitmap;
  TBool   mCached;
};

BResourceManager::BResourceManager(TAny *aROM) {
  TUint32 *ptr = (TUint32 *) aROM;
  this->mNumResources  = *ptr++;
  this->mResourceTable = ptr;
  this->mROM           = (TUint8 *) &ptr[this->mNumResources];
  for (TInt i = 0; i < MAX_BITMAP_SLOTS; i++) {
    mBitmapSlots[i] = ENull;
  }
//  printf("Number of resources: %d\n", this->mNumResources);
//  printf("Start: %lx End: %lx\n", binary_Resources_bin_start, binary_Resources_bin_end);
#if 0
  for (TInt i=0; i<this->mNumResources; i++) {
    bitmaps[i] = new BBitmap(&this->mROM[this->mResourceTable[i]]);
  }
#endif
}

BResourceManager::~BResourceManager() {
  ClearCache();
  ReleaseBitmaps();
}

// Load a bitmap from FLASH/ROM/RODATA into a slot
TBool BResourceManager::LoadBitmap(TInt16 aResourceId, TInt16 aSlotId, TInt16 aImageType) {
  BitmapSlot *slot = mBitmapSlots[aSlotId];
  if (slot) {
    return slot->mResourceId == aResourceId;
  }
  auto *bm = new BBitmap(&this->mROM[this->mResourceTable[aResourceId]]);
  mBitmapSlots[aSlotId] = new BitmapSlot(aResourceId, aImageType, bm);
  return ETrue;
}

TBool BResourceManager::SetBitmap(BBitmap *aBitmap, TInt16 aSlotId, TInt16 aImageType) {
  BitmapSlot *slot = mBitmapSlots[aSlotId];
  if (slot) {
    return slot->mBitmap == aBitmap;
  }
  mBitmapSlots[aSlotId] = new BitmapSlot(-1, aImageType, aBitmap);
  return ETrue;
}

TBool BResourceManager::ReleaseBitmap(TInt16 aSlotId) {
  BitmapSlot *slot = mBitmapSlots[aSlotId];
  if (!slot || slot->mCached) {
    return EFalse;
  }
  delete slot->mBitmap;
  delete slot;
  mBitmapSlots[aSlotId] = ENull;
  return ETrue;
}

void BResourceManager::ReleaseBitmaps() {
  for (TInt16 bm=0; bm<MAX_BITMAP_SLOTS; bm++) {
    ReleaseBitmap(bm);
  }
}

TBool BResourceManager::CacheBitmapSlot(TInt16 aSlotId, TBool aCacheIt) {
  BitmapSlot *slot = mBitmapSlots[aSlotId];
  if (!slot) {
    return EFalse;
  }
  slot->mCached = aCacheIt;
  return ETrue;
}

void BResourceManager::ClearCache(TBool aCacheIt) {
  for (TInt16 bm=0; bm<MAX_BITMAP_SLOTS; bm++) {
    CacheBitmapSlot(bm, aCacheIt);
  }
}

BBitmap *BResourceManager::GetBitmap(TInt16 aSlotId) {
  BitmapSlot *slot = mBitmapSlots[aSlotId];
  if (!slot) {
    return ENull;
  }
  return slot->mBitmap;
}

TInt BResourceManager::BitmapWidth(TInt aSlotId) {
  static const TUint8 widthTable[] = {
    0,8,16,32,64,8,16,8,32,8,64,16,32,16,64,32,64,32
  };
  BitmapSlot *slot = mBitmapSlots[aSlotId];
  if (!slot) {
    return 0;
  }

  return (slot->mImageType == IMAGE_ENTIRE) ? slot->mBitmap->Width() : TInt(widthTable[slot->mImageType]);
}

TInt BResourceManager::BitmapHeight(TInt aSlotId) {
  static const TUint8 heightTable[] = {
    0,8,16,32,64,16,8,32,8,64,8,32,16,64,16,64,32,40
  };
  BitmapSlot *slot = mBitmapSlots[aSlotId];
  if (!slot) {
    return 0;
  }

  return (slot->mImageType == IMAGE_ENTIRE) ?slot->mBitmap->Height() : TInt(heightTable[slot->mImageType]);
}
