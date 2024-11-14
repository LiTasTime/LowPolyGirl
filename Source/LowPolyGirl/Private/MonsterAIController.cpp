#include "MonsterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "LowPolyGirl/LowPolyGirlCharacter.h"

AMonsterAIController::AMonsterAIController()
{
    PrimaryActorTick.bCanEverTick = true;
    bIsPlayerLooking = false;
}

void AMonsterAIController::BeginPlay()
{
    Super::BeginPlay();

    // �������� ������ �� ������ � ��� ������
    PlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (PlayerCharacter)
    {
        ALowPolyGirlCharacter* LowPolyGirl = Cast<ALowPolyGirlCharacter>(PlayerCharacter);
        if (LowPolyGirl)
        {
            PlayerCamera = LowPolyGirl->FollowCamera;
        }
    }

    // �������� �������� � ������
    MoveToPlayer();
}

void AMonsterAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ���������, ������� �� ����� �� �������
    CheckIfPlayerIsLooking();

    APawn* ControlledPawn = GetPawn();
    if (ControlledPawn)
    {
        UCharacterMovementComponent* MovementComponent = ControlledPawn->FindComponentByClass<UCharacterMovementComponent>();

        // ������������� ��� ������������ �������� � ����������� �� ��������� �������
        if (bIsPlayerLooking)
        {
            MovementComponent->DisableMovement();  // ������������� ��������
        }
        else
        {
            MovementComponent->SetMovementMode(MOVE_Walking);  // ������������ ��������
        }
    }
}

void AMonsterAIController::CheckIfPlayerIsLooking()
{
    if (!PlayerCamera || !GetPawn()) return;

    FVector CameraLocation = PlayerCamera->GetComponentLocation();
    FVector CameraForward = PlayerCamera->GetForwardVector();
    FVector MonsterLocation = GetPawn()->GetActorLocation();

    // ������������ ����������� �� ������ �� �������
    FVector ToMonster = (MonsterLocation - CameraLocation).GetSafeNormal();
    float DotProduct = FVector::DotProduct(CameraForward, ToMonster);

    // ��������, ������� �� ����� �� �������
    bool bIsFacingMonster = (DotProduct > AngleThreshold);

    // ���������� Sweep (������ LineTrace), ����� ������ �������, ������� ����� ���� � ����
    TArray<FHitResult> HitResults;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(PlayerCharacter);  // ���������� ������
    Params.AddIgnoredActor(GetPawn());  // ���������� �������� �������

    // ���������� ������ �������, ����� ������� � ������
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterAIController::StaticClass(), FoundActors);
    Params.AddIgnoredActors(FoundActors);

    FVector EndLocation = CameraLocation + (ToMonster * TraceDistance);

    // ���������� Sweep, ������� ����� ���������� ������������ �� ����
    bool bHit = GetWorld()->SweepMultiByChannel(HitResults, CameraLocation, EndLocation, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(10.0f), Params);

    // �������� ��������� �������
    bool bMonsterVisible = bIsFacingMonster && HitResults.Num() == 0 && GetPawn()->WasRecentlyRendered();

    // ��������� ��������� bIsPlayerLooking
    bIsPlayerLooking = bMonsterVisible;

    // ��� �������
    /*if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("DotProduct: %f"), DotProduct));
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Player Looking: %s"), bIsPlayerLooking ? TEXT("True") : TEXT("False")));
    }

    // ���������� ����� �����������
    DrawDebugLine(GetWorld(), CameraLocation, MonsterLocation, FColor::Green, false, 0.1f, 0, 2.0f);*/
}

void AMonsterAIController::MoveToPlayer()
{
    if (PlayerCharacter && GetPawn())
    {
        MoveToActor(PlayerCharacter);
        GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, this, &AMonsterAIController::MoveToPlayer, 0.2f, false);
    }
}