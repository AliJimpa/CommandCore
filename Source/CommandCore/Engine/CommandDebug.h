#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(Log_CommandCore, Log, All);

#if !UE_BUILD_SHIPPING

#define LOG(Format, ...) \
    do { UE_LOG(Log_CommandCore, Log, TEXT(Format), ##__VA_ARGS__); } while (0)

#define LOG_WARNING(Format, ...) \
    do { UE_LOG(Log_CommandCore, Warning, TEXT(Format), ##__VA_ARGS__); } while (0)

#define LOG_ERROR(Format, ...) \
    do { UE_LOG(Log_CommandCore, Error, TEXT(Format), ##__VA_ARGS__); } while (0)

#define PRINT(Format, ...)                                     \
    do                                                         \
    {                                                          \
        if (GEngine)                                           \
        {                                                      \
            GEngine->AddOnScreenDebugMessage(                  \
                -1, 5.0f, FColor::Black,                       \
                FString::Printf(TEXT(Format), ##__VA_ARGS__)); \
        }                                                      \
    } while (0)

#else

#define LOG(...)           do {} while (0)
#define LOG_WARNING(...)   do {} while (0)
#define LOG_ERROR(...)     do {} while (0)
#define PRINT(...)         do {} while (0)

#endif