#ifndef PROSPECT_TI_H
#define PROSPECT_TI_H

#include "struct_comum.h"

#include <QMessageBox>
#include <QSqlDatabase>
#include <QInputDialog>
#include <QSqlQuery>
#include <QDialog>
#include <QDate>
#include <QtSql>
#include <QUrl>



namespace Ui {
class prospect_ti;
}

class prospect_ti : public QDialog
{
    Q_OBJECT

public:
    explicit prospect_ti(SessaoUsuario sessao, QWidget *parent = nullptr);
    ~prospect_ti();

private slots:
    void on_bt_voltar_clicked();
    void atualizar();
    void on_bt_pesquisar_clicked();
    void on_bt_pesquisar_cnpj_clicked();
    void on_bt_limpar_filtro_clicked(); //ver todos
    void on_bt_novo_clicked();
    void on_bt_editar_clicked();
    void on_bt_limpar_filtro_2_clicked(); // limpar filtro e mostrar só os últimos 400

private:
    Ui::prospect_ti *ui;

    SessaoUsuario m_dados;

};

#endif // PROSPECT_TI_H
