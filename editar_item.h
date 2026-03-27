#ifndef EDITAR_ITEM_H
#define EDITAR_ITEM_H

#include "struct_comum.h"

#include <QDialog>
#include <QSqlError>

namespace Ui {
class editar_item;
}

class editar_item : public QDialog
{
    Q_OBJECT

public:
    explicit editar_item(Sessao_editar_item sessaoE,QWidget *parent = nullptr);
    ~editar_item();

    Sessao_editar_item dados_item;


private slots:
    void on_bt_voltar_clicked();
    void on_txt_desconto_editingFinished();
    void on_txt_precoUnit_editingFinished();
    void on_txt_qtde_editingFinished();
    void on_txt_ipi_editingFinished();
    void on_bt_salvar_clicked();
    void desconto();
    void on_bt_D1_clicked();
    void on_bt_D2_clicked();
    void on_bt_D3_clicked();
    void on_bt_D4_clicked();
    void on_bt_D5_clicked();
    void on_bt_D6_clicked();
    void on_bt_D7_clicked();

private:
    Ui::editar_item *ui;


};

#endif // EDITAR_ITEM_H
