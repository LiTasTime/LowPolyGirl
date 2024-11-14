#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Camera/CameraComponent.h"  // ��������� ��� UCameraComponent
#include "GameFramework/Character.h" // ��������� ��� ACharacter
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

    // ��������� ��� LineTrace, ����� � ����� ���� �������� � Blueprint
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
    float TraceDistance = 1500.0f;

    // ���� ��� �������� ����������� �������
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
    float AngleThreshold = 0.7f;  // �������� 45 ��������

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
    float MinDistanceToStop = 100.0f;

private:
    void CheckIfPlayerIsLooking();
    void MoveToPlayer();

    bool bIsPlayerLooking;

    // ������ �� ������ � ��� ������
    ACharacter* PlayerCharacter;
    UCameraComponent* PlayerCamera;

    // ������ ��� ���������� ��������
    FTimerHandle MovementTimerHandle;
};