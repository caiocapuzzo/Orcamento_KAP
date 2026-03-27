#ifndef PROSPECT_NOVO_H
#define PROSPECT_NOVO_H

#include "struct_comum.h"

#include <QDesktopServices>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDialog>
#include <QDate>
#include <QUrl>



namespace Ui {
class prospect_novo;
}

class prospect_novo : public QDialog
{
    Q_OBJECT

public:
    explicit prospect_novo(SessaoUsuario sessao, QWidget *parent = nullptr);
    ~prospect_novo();

private slots:
    void on_bt_voltar_clicked();
    void on_txt_cnpj_editingFinished();
    void on_txt_cep_editingFinished();
    void on_bt_salvar_clicked();
    void on_bt_correio_2_clicked();
    void on_txt_nome_editingFinished();
    void on_txt_fantasia_editingFinished();
    void on_txt_cpf_editingFinished();

private:
    Ui::prospect_novo *ui;
     SessaoUsuario m_dados;

};

#endif // PROSPECT_NOVO_H
