// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjetProComp_M1Character.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "PowerComponent.h"
#include "PowerBase.h"
#include "PowerConfig.h"
#include "FirePower.h"
#include "IcePower.h"
#include "LightningPower.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AProjetProComp_M1Character::AProjetProComp_M1Character()
{
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw   = false;
    bUseControllerRotationRoll  = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    PowerComp = CreateDefaultSubobject<UPowerComponent>(TEXT("PowerComponent"));
}

void AProjetProComp_M1Character::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (ULocalPlayer* LP = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
            {
                FModifyContextOptions Opts;
                Opts.bIgnoreAllPressedKeysUntilRelease = false;
                Opts.bNotifyUserSettings = true;
                Subsystem->AddMappingContext(DefaultMappingContext, 0, Opts);
            }
        }
    }

    if (ensure(PowerComp))
    {
        if (!FirePowerClass)      FirePowerClass      = UFirePower::StaticClass();
        if (!IcePowerClass)       IcePowerClass       = UIcePower::StaticClass();
        if (!LightningPowerClass) LightningPowerClass = ULightningPower::StaticClass();

        int32 Count = 0;

        if (FireConfig.IsValid() || !FireConfig.ToSoftObjectPath().IsNull())
            if (UPowerConfig* DA = FireConfig.LoadSynchronous())
                { PowerComp->AddPower(FirePowerClass, DA); ++Count; }

        if (IceConfig.IsValid() || !IceConfig.ToSoftObjectPath().IsNull())
            if (UPowerConfig* DA = IceConfig.LoadSynchronous())
                { PowerComp->AddPower(IcePowerClass, DA); ++Count; }

        if (LightningConfig.IsValid() || !LightningConfig.ToSoftObjectPath().IsNull())
            if (UPowerConfig* DA = LightningConfig.LoadSynchronous())
                { PowerComp->AddPower(LightningPowerClass, DA); ++Count; }

        UE_LOG(LogTemplateCharacter, Log, TEXT("[Powers] Initialisés: %d pouvoirs, aucun n'est équipé par défaut."), Count);
    }
}

void AProjetProComp_M1Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (JumpAction)   { EIC->BindAction(JumpAction,   ETriggerEvent::Started,   this, &ACharacter::Jump);
                            EIC->BindAction(JumpAction,   ETriggerEvent::Completed, this, &ACharacter::StopJumping); }
        if (MoveAction)   { EIC->BindAction(MoveAction,   ETriggerEvent::Triggered, this, &AProjetProComp_M1Character::Move); }
        if (LookAction)   { EIC->BindAction(LookAction,   ETriggerEvent::Triggered, this, &AProjetProComp_M1Character::Look); }
        if (AbilityAction){ EIC->BindAction(AbilityAction,ETriggerEvent::Started,   this, &AProjetProComp_M1Character::OnAbility); }
        if (Slot1Action)  { EIC->BindAction(Slot1Action,  ETriggerEvent::Started,   this, &AProjetProComp_M1Character::OnSlot1); }
        if (Slot2Action)  { EIC->BindAction(Slot2Action,  ETriggerEvent::Started,   this, &AProjetProComp_M1Character::OnSlot2); }
        if (Slot3Action)  { EIC->BindAction(Slot3Action,  ETriggerEvent::Started,   this, &AProjetProComp_M1Character::OnSlot3); }
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' needs Enhanced Input component."), *GetNameSafe(this));
    }
}

void AProjetProComp_M1Character::Move(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    if (Controller)
    {
        const FRotator Rot = Controller->GetControlRotation();
        const FRotator YawRot(0, Rot.Yaw, 0);
        const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        const FVector Right   = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
        AddMovementInput(Forward, Axis.Y);
        AddMovementInput(Right,   Axis.X);
    }
}

void AProjetProComp_M1Character::Look(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    if (Controller)
    {
        AddControllerYawInput(Axis.X);
        AddControllerPitchInput(Axis.Y);
    }
}

void AProjetProComp_M1Character::OnAbility(const FInputActionValue& /*Value*/)
{
    if (PowerComp) { PowerComp->ActivateMain(); }
}

void AProjetProComp_M1Character::OnSlot1(const FInputActionValue& /*Value*/)
{
    if (PowerComp) { PowerComp->ChoosePower(EPowerElement::Fire); }
}

void AProjetProComp_M1Character::OnSlot2(const FInputActionValue& /*Value*/)
{
    if (PowerComp) { PowerComp->ChoosePower(EPowerElement::Ice); }
}

void AProjetProComp_M1Character::OnSlot3(const FInputActionValue& /*Value*/)
{
    if (PowerComp) { PowerComp->ChoosePower(EPowerElement::Lightning); }
}
