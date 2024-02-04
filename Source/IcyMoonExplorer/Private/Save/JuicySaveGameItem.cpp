#include "Save/JuicySaveGameItem.h"

FJuicySaveGameItem::FJuicySaveGameItem(const TSubclassOf<AActor> Class,
                                       const int32 CurrentStack)
	: Class{Class},
	  CurrentStack{CurrentStack}
{
}

FJuicySaveGameItem::~FJuicySaveGameItem()
{
}
