/*
 * ExtraChain Core
 * Copyright (C) 2020 ExtraChain Foundation <extrachain@gmail.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include "managers/contract_manager.h"

// ContractManager::ContractManager(AccountController *accountController, Blockchain *blockchain)
//{
//    this->accountController = accountController;
//    this->blockchain = blockchain;
//}

// void ContractManager::saveContract(const Contract &contract)
//{
//    QString path = based_dfs_struct::ROOT_FOOLDER_NAME + "/"
//        + accountController->getMainActor()->getId().toActorId() + "/" + "CONTRACTS" + "/";
//    qDebug() << path;
//    QDir().mkpath(path);

//    QFile contract_file(path + QString(contract.getFileName()));
//    contract_file.open(QIODevice::WriteOnly);
//    contract_file.write(contract.serialize());
//    contract_file.close();
//}

// void ContractManager::process()
//{
//}

// void ContractManager::loadContracts()
//{
//    contractList.clear();
//    QString path = based_dfs_struct::ROOT_FOOLDER_NAME + "/"
//        + accountController->getMainActor()->getId().toActorId() + "/" + "CONTRACTS" + "/";
//    QDir directory(path);
//    if (!directory.exists())
//        return;

//    QStringList contracts = directory.entryList(QDir::Files);
//    for (QString filename : contracts)
//    {
//        QFile open_file(path + filename);
//        open_file.open(QIODevice::ReadOnly);
//        contractList.append(Contract(open_file.readAll()));
//    }
//    for (auto temp : contractList)
//    {
//        qDebug() << temp.serialize();
//    }
//}

// void ContractManager::loadContractsFrom()
//{
//    loadContracts();
//    emit loadToUi(&contractList, accountController->getMainActor()->getId());
//}

// void ContractManager::createContract(const Contract &contract)
//{
//    contractList.append(contract);
//    contractList.last().signByCustomer(*accountController->getMainActor());

//    saveContract(contract);

//    emit contractIsCreated(contractList.last());
//    emit addContractToUi(contractList.last());
//    qDebug() << "contract is added!";
//}

// void ContractManager::updateContract(const Contract &contract)
//{
//    saveContract(contract);

//    emit contractIsCreated(contract);
//    emit addContractToUi(contract);
//    qDebug() << "contract is added!";
//}

// void ContractManager::addContractToManager(const Contract &contract)
//{
//    for (auto curr : contractList)
//    {
//        if (curr == contract)
//            return;
//    }
//    if (!contract.getCustomer_sign().isEmpty() && !contract.getPerformer_sign().isEmpty()
//        && contract.getFirst_transaction_hash().isEmpty()
//        && contract.getCustomer() == accountController->getMainActor()->getId())
//    {
//        emit makeFirstContractTransaction(contract);
//        return;
//    }
//    contractList.append(contract);
//    saveContract(contract);
//    loadContractsFrom();
//    // update ui
//}

// void ContractManager::contractFromNetWork(const Contract &contract)
//{
//    if (contract.getCustomer() == accountController->getMainActor()->getId()
//        || contract.getPerformer() == accountController->getMainActor()->getId())
//    {
//        qDebug() << "add contract";
//        addContractToManager(contract);
//        // do some staff
//    }

//    //    if (contract.getCustomer() == accountController->getMainActor()->getId()) {
//    //        if (contract.ver)
//    //    }
//}

// void ContractManager::approveContractByPerformer(QByteArray hash)
//{
//    qDebug() << "ContractManager::approveContractByPerformer ";
//    for (auto curr : contractList)
//    {
//        if (curr.getHash() == hash)
//        {
//            curr.signByPerformer(*accountController->getMainActor());
//            saveContract(curr);
//            contractIsCreated(curr);
//            break;
//        }
//    }
//}

// void ContractManager::completeContractByCustomer(QByteArray hash)
//{
//    qDebug() << "ContractManager::completeContractByCustomer ";
//    for (auto curr : contractList)
//    {
//        if (curr.getHash() == hash)
//        {
//            curr.completeContractByCustomer();
//            saveContract(curr);
//            contractIsCreated(curr);
//            break;
//        }
//    }
//}

// void ContractManager::completeContractByPerformer(QByteArray hash)
//{
//    qDebug() << "ContractManager::completeContractByPerformer ";
//    for (auto curr : contractList)
//    {
//        if (curr.getHash() == hash)
//        {
//            curr.completeContractByPerformer();
//            saveContract(curr);
//            contractIsCreated(curr);
//            makeFinalContractTransaction(curr);
//            break;
//        }
//    }
//}
