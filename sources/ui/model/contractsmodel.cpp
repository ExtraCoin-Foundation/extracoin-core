#include "ui/model/contractsmodel.h"

#include <QDebug>
#include <QPair>

#include "datastorage/contract.h"

ContractsModel::ContractsModel(AbstractModel *parent)
    : AbstractModel(parent)
{
    setModelRoles({ "type", "customer", "performer", "location", "event", "event_date_start",
                    "event_date_end", "agreement", "amount", "status", "statusId", "hash" });
    // statusId:
    // Not approved - 0
    // Approved - 1
    // Finished - 2
}

void ContractsModel::loadContracts()
{
}

void ContractsModel::createContract(QVariantMap contractData)
{
    /*
    QList<QByteArray> temp;
    for (auto t : contractData["scope_of_work"].toStringList())
        temp.append(t.toUtf8());
    Contract newContract(
        contractData["customer"].toByteArray(), contractData["performer"].toByteArray(),
        contractData["location"].toByteArray(), contractData["event"].toByteArray(),
        qMakePair(contractData["event_date_start"].toLongLong(), contractData["event_date_end"].toLongLong()),
        temp, contractData["agreement"].toByteArray(), contractData["amount"].toInt());

    emit newContractToNode(newContract);
    */
}

void ContractsModel::setContractList(QList<Contract> *conList, BigNumber currentId)
{
    /*
    clear();
    for (auto currentContract : *conList)
    {

        QVariantMap contractListMap;
        contractListMap["type"] = "contract";
        contractListMap["customer"] = QString(currentContract.getCustomer().toByteArray());
        contractListMap["performer"] = QString(currentContract.getPerformer().toByteArray());
        contractListMap["location"] = currentContract.getLocation();
        contractListMap["event"] = currentContract.getEvent();
        contractListMap["event_date_start"] = currentContract.getEvent_date().first;
        contractListMap["event_date_end"] = currentContract.getEvent_date().second;
        contractListMap["agreement"] = currentContract.getAgreement();
        contractListMap["amount"] = QString(currentContract.getAmount().toByteArray());
        contractListMap["status"] = "noStatus";
        contractListMap["statusId"] = -1;

        // determine contract status
        if (currentContract.getCustomer() == currentId)
        {
            // customer case
            if (currentContract.getPerformer_sign().isEmpty()) // mean contract doesn't have performer sign
                                                               //-> performer not approve contract
            {
                contractListMap["status"] = UiContractStatus::CUSTOMER_WAIT_APPROVE;
                contractListMap["statusId"] = 0;
            }
            else if (currentContract.getIsCompleted())
            {
                contractListMap["status"] = UiContractStatus::CONTRACT_COMPLETE;
                contractListMap["statusId"] = 2;
            }
            else
            {
                contractListMap["status"] = UiContractStatus::CUSTOMER_COMPLETE_BUTTON;
                contractListMap["statusId"] = 1;
            }
        }
        else if (currentContract.getPerformer() == currentId)
        {
            // performer case
            if (currentContract.getPerformer_sign().isEmpty()) // perfomer not approve -> show button
            {
                contractListMap["status"] = UiContractStatus::PERFORMER_MAKE_APPROVE;
                contractListMap["statusId"] = 0;
            }
            else if (!currentContract.getApprove_complete_performer())
            { // perfomer not complete -> show button
                contractListMap["status"] = UiContractStatus::PERFORMER_COMPLETE_BUTTON;
                contractListMap["statusId"] = 1;
            }
            else if (currentContract.getIsCompleted())
            {
                contractListMap["status"] = UiContractStatus::CONTRACT_COMPLETE;
                contractListMap["statusId"] = 2;
            }
            else
            { // performer not complete,
                // cutommer complete ->
                // wait for customer approve complete
                contractListMap["status"] = UiContractStatus::PERFORMER_WAIT_COMPLETE;
                contractListMap["statusId"] = 1;
            }
        }

        contractListMap["hash"] = currentContract.getHash();

        //        for (int i = 0; i < count(); ++i)
        //        {
        //            if (currentContract.getHash() != get(i)["hash"])
        //            {
        //                continue;
        //            }
        //        }
        append(contractListMap);
    }
    */
}
