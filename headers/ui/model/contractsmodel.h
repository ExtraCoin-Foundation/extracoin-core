#ifndef ContractsModel_H
#define ContractsModel_H

#include "ui/model/abstractmodel.h"
#include "utils/bignumber.h"

class Contract;

namespace UiContractStatus {
static const QString CUSTOMER_WAIT_APPROVE = "customerWaitApprove";
static const QString PERFORMER_MAKE_APPROVE = "performerMakeApprove";
static const QString CUSTOMER_COMPLETE_BUTTON = "customerCompleteButton";
static const QString PERFORMER_COMPLETE_BUTTON = "performerCompleteButton";
static const QString PERFORMER_WAIT_COMPLETE = "performerWaitComplete";
static const QString CONTRACT_COMPLETE = "contractComplete";
}

class ContractsModel : public AbstractModel
{
    Q_OBJECT

public:
    explicit ContractsModel(AbstractModel *parent = nullptr);

    Q_INVOKABLE void loadContracts();
    Q_INVOKABLE void createContract(QVariantMap contractData);

    void setContractList(QList<Contract> *conList, BigNumber currentId);

signals:
    void addContract(QVariantMap);
    void newContractToNode(const Contract &contract);
    void loadContractst();

    // approve signal to node
    void approveByPerformer(QByteArray hash);  // PERFORMER_MAKE_APPROVE
    void completeByPerformer(QByteArray hash); // PERFORMER_COMPLETE_BUTTON
    void completeByCustomer(QByteArray hash);  // CUSTOMER_COMPLETE_BUTTON
};

#endif // ContractsModel_H
