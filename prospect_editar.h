#ifndef PROSPECT_EDITAR_H
#define PROSPECT_EDITAR_H

#include "struct_comum.h"

#include <QtSql>
#include <QDate>
#include <QDialog>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QDesktopServices>

namespace Ui {
class prospect_editar;
}

class prospect_editar : public QDialog
{
    Q_OBJECT

public:
    explicit prospect_editar(SessaoUsuario sessao, QWidget *parent = nullptr);
    ~prospect_editar();

private slots:
    void on_bt_voltar_clicked();

    void on_bt_salvar_clicked();

    void on_bt_correio_2_clicked();

    void on_txt_cep_editingFinished();

    void on_txt_cnpj_editingFinished();

    void on_txt_cpf_editingFinished();

private:
    Ui::prospect_editar *ui;
    SessaoUsuario m_dados;
};

#endif // PROSPECT_EDITAR_H
