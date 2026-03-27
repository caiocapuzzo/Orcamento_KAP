#include "pedidos.h"
#include "ui_pedidos.h"
#include "struct_comum.h"
#include "pedido_itens.h"


pedidos::pedidos(SessaoUsuario sessao, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::pedidos)
    , m_dbpModel(new QSqlTableModel(this,dbp))
    , m_dados(sessao) // Copia o pacote recebido para a variável m_dados

{
    ui->setupUi(this);


      this->showFullScreen();



    if(m_dados.vendedor == "000001"){
        QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
        QSqlQuery qry_vendedor(ProtheusDB);
        qry_vendedor.prepare("SELECT A3_COD,A3_NREDUZ,A3_MSBLQL "
                             "FROM SA3010 "
                             "WHERE A3_MSBLQL = '2' "
                             "ORDER BY A3_NREDUZ");
        if(!qry_vendedor.exec()){
            QMessageBox::warning(this,"FALHA","Falha na consulta qry_vendedor");
            return;
        }
        QStandardItemModel *modelve = new QStandardItemModel (this);
        while (qry_vendedor.next()) {
            modelve->appendRow(new QStandardItem(qry_vendedor.value(1).toString().trimmed()+ " - "+qry_vendedor.value(0).toString() ));
        }
        ui->cb_por_vendedor->setModel(modelve);
        ui->cb_por_vendedor->setCurrentIndex(-1);
    }
    ui->tw_coleta->setVisible(false);
    on_bt_limparfiltro_clicked();


}

pedidos::~pedidos()
{
    delete ui;
}

void pedidos::on_bt_voltar_clicked()
{
    close();
}

void pedidos::on_bt_limparfiltro_clicked()
{
    QString vendedor = m_dados.vendedor.trimmed();
    QString regiao = m_dados.regiao.trimmed();
    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");

    QSqlQuery qry_pedidos(ProtheusDB);
    QString sql;

    QString colunas = "SELECT TOP %1 SC5010.C5_NUM, "
                      "Right(SC5010.[C5_EMISSAO],2) + '/'+ substring(SC5010.[C5_EMISSAO],5,2) + '/' + Left(SC5010.[C5_EMISSAO],4) AS C5_EMISSAO, "
                      "Right(SC5010.[C5_DTCLI],2) + '/'+ substring(SC5010.[C5_DTCLI],5,2) + '/' + Left(SC5010.[C5_DTCLI],4) AS C5_DTCLI, "
                      "SA1010.A1_COD, SA1010.A1_LOJA, SA1010.A1_NREDUZ, SA1010.A1_TIPO, SA1010.A1_CGC, "
                      "SC5010.C5_PEDCLI, SA4010.A4_NREDUZ, "
                      "IIF(SA1010.[A1_MSBLQL] = '1', 'SIM' , 'Não') AS A1_MSBLQL, "
                      "SC5010.C5_NOTA, SA1010.A1_EST, SC5010.C5_CONDPAG "
                      "FROM SC5010 "
                      "INNER JOIN SA1010 ON SC5010.C5_CLIENTE = SA1010.A1_COD AND SC5010.C5_LOJACLI = SA1010.A1_LOJA "
                      "INNER JOIN SA4010 ON SC5010.C5_TRANSP = SA4010.A4_COD "
                      "WHERE SC5010.C5_TIPO = 'N' AND SC5010.D_E_L_E_T_ = ' ' "
                      "AND SA4010.D_E_L_E_T_ = ' ' AND SA1010.D_E_L_E_T_ = ' ' ";

    if (vendedor == "000001") {
        sql = colunas.arg("4000");
    } else {
        sql = colunas.arg("1000") + "AND SA1010.A1_REGIAO LIKE '" + m_dados.regiao + "%' ";
    }

    sql += "ORDER BY SC5010.C5_NUM DESC";

    if (!qry_pedidos.exec(sql)) {
        QMessageBox::warning(this, "Falha", "Falha na consulta qry_pedidos");
        return;
    }

    ui->lb_qtde->clear();
    m_dbpModel->setQuery(qry_pedidos);

    QStringList headers = {
        "Nº Pedido", "Data \nemissão", "Data \ncliente", "Código do \ncliente",
        "Loja", "Fantasia", "Tipo", "CNPJ", "Pedido do \ncliente",
        "Transportadora", "Cliente\nBloqueado?", "Nota fiscal", "UF", "Código da\ncond. pgto"
    };

    for (int i = 0; i < headers.size(); ++i) {
        m_dbpModel->setHeaderData(i, Qt::Horizontal, headers.at(i));
    }

    ui->tv_pedidos->setModel(m_dbpModel);

    QHeaderView *hHeader = ui->tv_pedidos->horizontalHeader();
    QHeaderView *vHeader = ui->tv_pedidos->verticalHeader();

    hHeader->setStretchLastSection(true);
    ui->tv_pedidos->setSortingEnabled(true);
    hHeader->setSortIndicator(0, Qt::DescendingOrder);
    ui->tv_pedidos->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tv_pedidos->setSelectionBehavior(QAbstractItemView::SelectRows);
    vHeader->hide();

    QVector<int> columnWidths = {80, 80, 80, 80, 40, 180, 40, 110, 140, 120, 75, 90, 30, 70};
    for (int i = 0; i < 14; ++i) {
        ui->tv_pedidos->setColumnWidth(i, columnWidths[i]);
    }
    // Faz a coluna de Descrição (índice 4) ocupar o espaço restante se a janela crescer
    ui->tv_pedidos->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
}

void pedidos::on_bt_itens_do_pedido_clicked()
{
    int linha = ui->tv_pedidos->currentIndex().row() ;   // funciona
    int wpedido_itens = ui->tv_pedidos->model()->data(ui->tv_pedidos->model()->index(linha,0)).toInt() ;

    pedido_itens f_pedidos_itens(this,wpedido_itens);
    f_pedidos_itens.exec();
}

