#ifndef PHYSICS_LIST_HPP
#define PHYSICS_LIST_HPP

#include "G4VModularPhysicsList.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLowEPPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4StoppingPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4EmParameters.hh"
#include "G4ProductionCutsTable.hh"
#include "G4SystemOfUnits.hh"

class PhysicsList : public G4VModularPhysicsList {
public:
    PhysicsList() : cutForGamma(1*mm), cutForElectron(1*mm), cutForPositron(1*mm) {
        // Устанавливаем вербальность для отладки
        SetVerboseLevel(1);
        
        // Регистрируем стандартные физические процессы
        RegisterPhysics(new G4DecayPhysics());
        RegisterPhysics(new G4RadioactiveDecayPhysics());
        RegisterPhysics(new G4EmExtraPhysics());
        RegisterPhysics(new G4HadronElasticPhysics());
        RegisterPhysics(new G4HadronPhysicsFTFP_BERT());
        RegisterPhysics(new G4StoppingPhysics());
        RegisterPhysics(new G4IonPhysics());
        
        // Регистрируем электромагнитную физику с опцией 4 (оптимизирована для медицинской физики)
        RegisterPhysics(new G4EmStandardPhysics_option4());
        
        // Регистрируем низкоэнергетическую физику для точного моделирования на малых энергиях
        RegisterPhysics(new G4EmLowEPPhysics());
        
        // Настраиваем параметры электромагнитных процессов
        ConfigureEMPhysics();
    }
    
    virtual ~PhysicsList() {}
    
    virtual void SetCuts() override {
        // Устанавливаем пороги отсечки по умолчанию
        SetCutsWithDefault();
        
        // Устанавливаем специфичные пороги отсечки
        SetCutValue(cutForGamma, "gamma");
        SetCutValue(cutForElectron, "e-");
        SetCutValue(cutForPositron, "e+");
        
        // Выводим информацию о порогах отсечки
        if (verboseLevel > 0) {
            DumpCutValuesTable();
        }
        
        // Обновляем таблицу порогов отсечки
        G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(1000*eV, 1*GeV);
    }
    
    void SetGammaCut(G4double cut) { cutForGamma = cut; }
    void SetElectronCut(G4double cut) { cutForElectron = cut; }
    void SetPositronCut(G4double cut) { cutForPositron = cut; }
    
    G4double GetGammaCut() const { return cutForGamma; }
    G4double GetElectronCut() const { return cutForElectron; }
    G4double GetPositronCut() const { return cutForPositron; }

private:
    void ConfigureEMPhysics() {
        // Получаем параметры EM процессов
        G4EmParameters* params = G4EmParameters::Instance();
        
        // Настраиваем параметры для точного моделирования дозimetry
        params->SetDefaults();
        params->SetVerbose(1);
        
        // Включаем точное вычисление потерь энергии
        params->SetLossFluctuations(true);
        /*
        Расчёт энергий частиц с точки зрения ЭМ модели тратить крайне много
        ресурсов. По большей степени вследствии численого расчёта
        3(возможно 4-)-интеграла по плотности заряда.
        
        Функция необходима для отключения трекинга частиц с энергией ниже указаной.
        Разница энергии передаётся среде для соблюдения ЗСЭ.

        Минимального значения формально нет.
        */
        params->SetMinEnergy(100*eV);        // Минимальная энергия для трекинга
        params->SetMaxEnergy(10*GeV);        // Максимальная энергия
        params->SetNumberOfBinsPerDecade(20); // Количество бинов на декаду энергии
        
        // Настраиваем параметры для точного моделирования в области низких энергий
        params->SetLowestElectronEnergy(100*eV);
        
        // Включаем дополнительные опции для медицинской физики
        params->SetUseICRU90Data(true);      // Используем данные ICRU 90
        params->SetApplyCuts(true);          // Применяем пороги отсечки
        params->SetStepFunction(0.01, 0.1*mm); // Функция шага: dE/dx = 0.01, min step = 0.1mm
        
        // Настраиваем параметры для моделирования вторичных частиц
        params->SetBuildCSDARange(true);
        params->SetMaxNIELEnergy(1*MeV);
        
        // Включаем точное моделирование для электронов
        params->SetUseMottCorrection(true);  // Поправка Мотта для электронов
        
        // Дополнительные параметры для низкоэнергетической физики
        params->SetAuger(true);              // Включаем эффект Оже
        params->SetPixe(true);               // Включаем PIXE (рентгеновское излучение протонов)
        params->SetDeexcitationIgnoreCut(true);
        // params->SetAuger(false);
        // params->SetPixe(false);
        // params->SetDeexcitationIgnoreCut(false);
        
        if (verboseLevel > 0) {
            G4cout << "EM physics configured for low-energy radiation studies" << G4endl;
            G4cout << "Min energy: " << params->MinKinEnergy()/keV << " keV" << G4endl;
            G4cout << "Max energy: " << params->MaxKinEnergy()/MeV << " MeV" << G4endl;
        }
    }

private:
    G4double cutForGamma;
    G4double cutForElectron;
    G4double cutForPositron;
};

#endif // PHYSICS_LIST_HPP