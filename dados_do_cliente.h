#ifndef DADOS_DO_CLIENTE_H
#define DADOS_DO_CLIENTE_H

#include "struct_comum.h"

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDialog>
#include <QDate>



namespace Ui {
class dados_do_cliente;
}



class dados_do_cliente : public QDialog
{
    Q_OBJECT

public:
    explicit dados_do_cliente(Sessao_dados_do_cliente sessaoC, QWidget *parent = nullptr);
    ~dados_do_cliente();

   Sessao_dados_do_cliente dados_cliente;

private slots:
    void on_bt_voltar_clicked();
    QString deWindowsParaUtf8(const QVariant &v);

private:
    Ui::dados_do_cliente *ui;
};

#endif // DADOS_DO_CLIENTE_H
