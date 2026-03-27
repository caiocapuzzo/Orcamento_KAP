#ifndef PEDIDO_ITENS_H
#define PEDIDO_ITENS_H

#include <QDialog>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlTableModel>



namespace Ui {
class pedido_itens;
}

class pedido_itens : public QDialog
{
    Q_OBJECT

public:
    explicit pedido_itens(QWidget *parent = nullptr, int wpedido_itens = 0);
    ~pedido_itens();

private slots:
    void on_bt_voltar_clicked();

private:
    Ui::pedido_itens *ui;

    QSqlDatabase dbp;
    QSqlQueryModel * m_dbp_itensModel;

};

#endif // PEDIDO_ITENS_H
