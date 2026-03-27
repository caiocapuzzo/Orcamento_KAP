#ifndef ORCAMENTO_EDITAR_H
#define ORCAMENTO_EDITAR_H


#include "struct_comum.h"

#include <QStandardItemModel>
#include <QDesktopServices>  // para o desenho
#include <QSqlQueryModel>
#include <QStandardItem>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QTableView>
#include <QKeyEvent>
#include <QSqlQuery>
#include <QSqlError>
#include <QPixmap>
#include <QDebug>
#include <QDialog>
#include <QDate>
#include <QDir>



namespace Ui {
class orcamento_editar;
}

class orcamento_editar : public QDialog
{
    Q_OBJECT

public:
    explicit orcamento_editar(SessaoUsuario sessao, QWidget *parent = nullptr);
    ~orcamento_editar();

      int contLi = 0;

private slots:
    void on_bt_voltar_clicked();
    void on_txt_nomeCli_editingFinished();
    void cliloja();
    void preencherCamposCliente(const QSqlQuery &q);
    void configurarCabecalhosCliLoja();
    void on_txt_codCli_editingFinished();
    void on_txt_cnpjCli_editingFinished();
    void on_txt_prospect_editingFinished();
    void prospect_cliloja();
    void on_txt_codprospect_editingFinished();
    void on_txt_cnpj_prospect_editingFinished();
    void on_txt_cpf_prospect_editingFinished();
    void on_txt_qtde_editingFinished();
    void on_txt_codP_editingFinished();
    void Sub_somatorio_dos_precos_e_pesos();
    QString buscar_cod_kap();
    int sub_tempo_TS();
    void on_bt_correios_clicked();
    int calcularPrazoEstrutura(QString codPai, int qtdeNecessariaPai, QStandardItemModel *model_estru = nullptr);
    void estoqueS();
    void on_bt_desenho_clicked();
    void on_bt_dadosdo_cliente_clicked();
    void on_bt_excluiritem_clicked();
    void on_bt_abrirOP_clicked();
    void on_bt_editaritem_clicked();
    void on_bt_oquecomprou_clicked();
    void on_bt_ultimasVendas_clicked();
    void on_bt_prazo_iguais_clicked();
    void on_bt_desconto_geral_clicked();
    void on_txt_codP_no_cli_editingFinished();
    void on_txt_cod_concorrente_editingFinished();
    void on_similar();
    void keyPressEvent(QKeyEvent*ke);
    void on_tw_produtos_cellClicked(int row, int column);
    void on_tw_produtos_cellDoubleClicked(int row, int column);
    void on_bt_salvar_clicked();
   // void on_bt_dadosdo_cliente_clicked();
    bool validarCamposObrigatorios();

private:
    Ui::orcamento_editar *ui;
    SessaoUsuario m_dados;

    QString m_tipo;
    QString m_grupo;
    QString m_p_endereco;
    QString m_p_bairro;
    QString m_p_cidade;
    QString m_valorTotal_2;
    QString m_item_p_img_semelhante;
    QString m_contato_original;
    QString m_email_original;
    QString m_ddd_original;
    QString m_telefone_original;

    QTableView *tv_ultimasVendas; // Declare o ponteiro aqui
    QStandardItemModel *model_uv; // Guardar o model também ajuda na performance

    QTableView *tv_oqueComprou;

};

#endif // ORCAMENTO_EDITAR_H
