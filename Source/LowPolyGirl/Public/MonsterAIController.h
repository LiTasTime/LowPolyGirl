#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Camera/CameraComponent.h"  // Добавляем для UCameraComponent
#include "GameFramework/Character.h" // Добавляем для ACharacter
#include "MonsterAIController.generated.h"

UCLASS()
class LOWPOLYGIRL_API AMonsterAIController : public AAIController
{
    GENERATED_BODY()

public:
    AMonsterAIController();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // Дистанция для LineTrace, чтобы её можно было изменять в Blueprint
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
    float TraceDistance = 1500.0f;

    // Угол для проверки направления взгляда
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
    float AngleThreshold = 0.7f;  // Примерно 45 градусов

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
    float MinDistanceToStop = 100.0f;

private:
    void CheckIfPlayerIsLooking();
    void MoveToPlayer();

    bool bIsPlayerLooking;

    // Ссылка на игрока и его камеру
    ACharacter* PlayerCharacter;
    UCameraComponent* PlayerCamera;

    // Таймер для повторного движения
    FTimerHandle MovementTimerHandle;
};