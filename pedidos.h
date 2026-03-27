#ifndef PEDIDOS_H
#define PEDIDOS_H

#include "struct_comum.h"

#include <QStyleOptionViewItem>
#include <QStandardItemModel>
#include <QSqlTableModel>
#include <QMessageBox>
#include <QInputDialog>
#include <QModelIndex>
#include <QSqlQuery>
#include <QDebug>
#include <QDialog>
#include <QColor>
#include <QDate>


namespace Ui {
class pedidos;
}

class pedidos : public QDialog
{
    Q_OBJECT

public:
    explicit pedidos(SessaoUsuario sessao, QWidget *parent = nullptr);
    ~pedidos();

private slots:
    void on_bt_voltar_clicked();
    void on_bt_limparfiltro_clicked();

    void on_bt_itens_do_pedido_clicked();

private:
    Ui::pedidos *ui;

    QSqlDatabase dbp;
    QSqlQueryModel * m_dbpModel;

    SessaoUsuario m_dados;

};

#endif // PEDIDOS_H
