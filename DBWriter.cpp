//
// Created by patrick on 1/1/17.
//


#include <iostream>
#include "DBWriter.h"
#include "GVAR.h"

using namespace::std;

DBWriter* DBWriter::writer = nullptr;

//����ģʽ
DBWriter *DBWriter::getInstance(){
    if (writer == nullptr){
        writer = new DBWriter();
        return writer;
    }
    else
    {
        return writer;
    }
}

DBWriter::DBWriter() {
    db = QSqlDatabase::addDatabase(DB_DRIVER_NAME);
    db.setHostName(DB_HOST_NAME);
    db.setDatabaseName(DATABASE_NAME);
    db.setUserName(USER_NAME);
    db.setPassword(PASSWORD);
    if (!db.open()) {
        cout << "���ݿ����,�޷������ݿ�";
        abort();
    }
}

void DBWriter::InsertTickData(QString tableName, CThostFtdcDepthMarketDataField *pDepthMarketData) {
    QSqlQuery insert;
    //24��
    insert.prepare("insert into " + tableName + " values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
    //�󶨲�ѯ
    QString tdate                 = pDepthMarketData->TradingDay;
    QString InstrumentID         = pDepthMarketData->InstrumentID;
    double LastPrice            = pDepthMarketData->LastPrice;
    double PreSetttlementPrice  = pDepthMarketData->PreSettlementPrice;
    double PreClosePrice        = pDepthMarketData->PreClosePrice;
    double PreOpenInterest      = pDepthMarketData->PreOpenInterest;
    double OpenPrice            = pDepthMarketData->OpenPrice;
    double HighestPrice         = pDepthMarketData->HighestPrice;
    double LowestPrice          = pDepthMarketData->LowestPrice;
    int    Volume               = pDepthMarketData->Volume;
    double Turnover             = pDepthMarketData->Turnover;//0
    double OpenInterest         = pDepthMarketData->OpenInterest; //0
    double ClosePrice           = pDepthMarketData->ClosePrice;
    double SettlementPrie       = pDepthMarketData->SettlementPrice;
    double UpperLimitPrice      = pDepthMarketData->UpperLimitPrice;
    double LowerLimitPirce      = pDepthMarketData->LowerLimitPrice;
    QString UpdateTime           = pDepthMarketData->UpdateTime;
    int    UpdateMillisec       = pDepthMarketData->UpdateMillisec;
    double BidPrice1            = pDepthMarketData->BidPrice1;
    int    BidVolume1           = pDepthMarketData->BidVolume1;
    double AskPrice1            = pDepthMarketData->AskPrice1;
    int    AskVolume1           = pDepthMarketData->AskVolume1;
    double AveragePrice         = pDepthMarketData->AveragePrice;
    QString ActionDay            = pDepthMarketData->ActionDay;

    insert.bindValue(0, tdate);
    insert.bindValue(1, InstrumentID);
    insert.bindValue(2, LastPrice);
    insert.bindValue(3, PreSetttlementPrice);
    insert.bindValue(4, PreClosePrice);
    insert.bindValue(5, OpenPrice);
    insert.bindValue(6, PreOpenInterest);
    insert.bindValue(7, HighestPrice);
    insert.bindValue(8, LowestPrice);
    insert.bindValue(9, Volume);
    insert.bindValue(10, Turnover);
    insert.bindValue(11, OpenInterest);
    insert.bindValue(12, ClosePrice);
    insert.bindValue(13, SettlementPrie);
    insert.bindValue(14, UpperLimitPrice);
    insert.bindValue(15, LowerLimitPirce);
    insert.bindValue(16, UpdateTime);
    insert.bindValue(17, UpdateMillisec);
    insert.bindValue(18, BidPrice1);
    insert.bindValue(19, BidVolume1);
    insert.bindValue(20, AskPrice1);
    insert.bindValue(21, AskVolume1);
    insert.bindValue(22, AveragePrice);
    insert.bindValue(23, ActionDay);

    bool result = insert.exec();
//    qDebug() << result;

}

void DBWriter::InsertOrUpdateAccountPostion(QString tableName, CThostFtdcInvestorPositionField *positionInfo) {
    //�ȼ���Ƿ���ڸ��˻��ĳֲּ�¼
    QSqlQuery exist(this->db);
    exist.prepare("select investor_id from account_position where investor_id=:id and instrument_id=:instru ");

    exist.bindValue(":id", positionInfo->InvestorID);
    exist.bindValue(":instru", positionInfo->InstrumentID);
    exist.exec();

    //qDebug() << "000:" << exist.lastError().databaseText();
    if (exist.next()) {
        //������ھ͸���
        QSqlQuery update(this->db);
        if (positionInfo->PosiDirection == THOST_FTDC_PD_Long) {    //��ͷ�ֲ�
            update.prepare(" update account_position set long_position=:long_position , long_profit=:long_profit, "
                                   " long_margin=:long_margin,query_date=:date where investor_id=:id and instrument_id=:instru ");
            update.bindValue(":long_position", positionInfo->Position);
            update.bindValue(":long_profit", positionInfo->PositionProfit);
            update.bindValue("long_margin", positionInfo->UseMargin);
            update.bindValue(":date", positionInfo->TradingDay);
            update.bindValue(":id", positionInfo->InvestorID);
            update.bindValue(":instru", positionInfo->InstrumentID);
            update.exec();
            //qDebug() << "111:"<<update.lastError().databaseText();
        }
        if (positionInfo->PosiDirection == THOST_FTDC_PD_Short) { //��ͷ�ֲ�
            update.prepare(" update account_position set short_position=:short_position,short_profit=:short_profit, "
                                   " short_margin=:short_margin,query_date=:date where investor_id=:id and instrument_id=:instru ");
            update.bindValue(":short_position", positionInfo->Position);
            update.bindValue(":short_profit", positionInfo->PositionProfit);
            update.bindValue(":short_margin", positionInfo->UseMargin);
            update.bindValue(":date", positionInfo->TradingDay);
            update.bindValue(":id", positionInfo->InvestorID);
            update.bindValue(":instru", positionInfo->InstrumentID);
            update.exec();
            //qDebug() << "222:" << update.lastError().databaseText();
        }
    } else {
        //����Ͳ���
        QSqlQuery insert(this->db);
        if (positionInfo->PosiDirection == THOST_FTDC_PD_Long) {    //��ͷ�ֲ�
            insert.prepare("insert into account_position (investor_id,instrument_id,long_position,long_profit, long_margin,query_date) values (:id,:instru,:long_position,:long_profit,:long_margin,:date) ");
            insert.bindValue(":id", positionInfo->InvestorID);
            insert.bindValue(":instru", positionInfo->InstrumentID);
            insert.bindValue(":long_position", positionInfo->Position);
            insert.bindValue(":long_profit", positionInfo->PositionProfit);
            insert.bindValue(":long_margin", positionInfo->UseMargin);
            insert.bindValue(":date", positionInfo->TradingDay);
//            cout << insert << endl;

            bool insert_result = insert.exec();
//            if(insert_result)
//            {
//                cout << "insert success" <<endl;
//            }
            //qDebug() << "333:" << insert.lastError().databaseText();
        }
        if (positionInfo->PosiDirection == THOST_FTDC_PD_Short) { //��ͷ�ֲ�
            insert.prepare("insert into account_position (investor_id,instrument_id,short_position,short_profit, "
                                   " short_margin,query_date) values (:id,:instru,:short_position,:short_profit,:short_margin,:date) ");
            insert.bindValue(":id", positionInfo->InvestorID);
            insert.bindValue(":instru", positionInfo->InstrumentID);
            insert.bindValue(":short_position", positionInfo->Position);
            insert.bindValue(":short_profit", positionInfo->PositionProfit);
            insert.bindValue(":short_margin", positionInfo->UseMargin);
            insert.bindValue(":date", positionInfo->TradingDay);
            insert.exec();
            //qDebug() << "444:" << insert.lastError().databaseText();
        }
    }
}

void DBWriter::DBClose(){
    QString connection;
    connection = db.connectionName();
    db.close();
    db.removeDatabase(connection);

}