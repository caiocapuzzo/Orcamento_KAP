#ifndef ATIVIDADES_H
#define ATIVIDADES_H
#include "struct_comum.h"


#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QMessageBox>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QDialog>





namespace Ui {
class atividades;
}

class atividades : public QDialog
{
    Q_OBJECT

public:
    explicit atividades(SessaoUsuario sessaoA, QWidget *parent = nullptr);
    ~atividades();

    SessaoUsuario dados_cliente;



private slots:
    void on_bt_voltar_clicked();
    void on_bt_pesquisarAtividade_clicked();
    void on_txt_p_atividade_textChanged(const QString &watividade);


private:
    Ui::atividades *ui;

    QSqlQueryModel * m_dbaModel;            //a de atividade
    QSortFilterProxyModel *m_proxyModel; // "filtro" intermediário


    // índices para os campos consulta atividades
    int i_codCliCol; // Código do cliente na tabela atividades
    int i_loja;
    int i_nome;
    int i_ativi;
    int i_prop;
    int i_multi;
    int i_orig;
};

#endif // ATIVIDADES_H
