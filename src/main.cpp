#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "DetectorConstruction.hpp"
#include "PhysicsList.hpp"
#include "PrimaryGeneratorAction.hpp"
#include "RunAction.hpp"
#include "SteppingAction.hpp"

int main(int argc, char** argv) {
    // Инициализация ядра Geant4
    G4RunManager* runManager = new G4RunManager();
    
    // Создание и установка обязательных классов
    DetectorConstruction* detector = new DetectorConstruction();
    runManager->SetUserInitialization(detector);
    
    PhysicsList* physicsList = new PhysicsList();
    runManager->SetUserInitialization(physicsList);
    
    // Инициализация ядра
    runManager->Initialize();
    
    // Создание пользовательских классов действий
    PrimaryGeneratorAction* generatorAction = new PrimaryGeneratorAction();
    runManager->SetUserAction(generatorAction);
    
    RunAction* runAction = new RunAction(detector);
    runManager->SetUserAction(runAction);
    
    SteppingAction* steppingAction = new SteppingAction(runAction, detector);
    runManager->SetUserAction(steppingAction);
    
    // Настройка визуализации и сессии
    G4VisManager* visManager = new G4VisExecutive();
    visManager->Initialize();
    
    G4UIExecutive* uiExecutive = new G4UIExecutive(argc, argv);
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    
    // Базовая конфигурация
    UImanager->ApplyCommand("/control/execute macros/init_vis.mac");
    
    // Запуск интерактивной сессии или выполнение macro-файла
    if (argc == 1) {
        // Интерактивный режим
        uiExecutive->SessionStart();
    } else {
        // Пакетный режим с macro-файлом
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command + fileName);
    }
    
    // Очистка памяти
    delete uiExecutive;
    delete visManager;
    delete runManager;
    
    return 0;
}