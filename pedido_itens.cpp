#include "pedido_itens.h"
#include "ui_pedido_itens.h"

pedido_itens::pedido_itens(QWidget *parent, int wpedido_itens )
    : QDialog(parent)
    , ui(new Ui::pedido_itens)
{
    ui->setupUi(this);

    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
    QSqlQuery qry(ProtheusDB);
    QLocale brasilLocale(QLocale::Portuguese, QLocale::Brazil);

    // 1. CONFIGURAÇÃO INICIAL DA TABELA
    ui->tw_pedido_itens->setSortingEnabled(false);
    ui->tw_pedido_itens->setColumnCount(11);
    ui->tw_pedido_itens->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tw_pedido_itens->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tw_pedido_itens->verticalHeader()->setVisible(false);
    ui->tw_pedido_itens->setHorizontalHeaderLabels({
        "Pedido", "Item", "Código Produto", "Cód. Cliente", "Descrição",
        "Quantidade", "Preço Unit.", "Sub Total (R$)", "Data", "NF", "Lote"
    });

    // 2. PRIMEIRA TENTATIVA: JOIN SC6 + SC9
    qry.prepare("SELECT SC6.C6_NUM, SC6.C6_ITEM, SC6.C6_PRODUTO, SC6.C6_DESCRI, "
                "SC9.C9_QTDLIB, SC6.C6_PRCVEN, SC6.C6_VALOR, SC6.C6_DATFAT, SC6.C6_NOTA, "
                "SC9.C9_LOTECTL, SC9.C9_CLIENTE, SC9.C9_LOJA "
                "FROM SC6010 SC6 "
                "LEFT JOIN SC9010 SC9 ON SC6.C6_PRODUTO = SC9.C9_PRODUTO AND SC6.C6_NUM = SC9.C9_PEDIDO "
                "WHERE SC6.C6_NUM = :pedido AND SC6.D_E_L_E_T_ = ' ' "
                "AND (SC9.D_E_L_E_T_ = ' ' OR SC9.D_E_L_E_T_ IS NULL) "
                "ORDER BY SC6.C6_ITEM");
    qry.bindValue(":pedido", QString::number(wpedido_itens));

    if (!qry.exec()) {
        QMessageBox::warning(this, "Falha", "Erro na consulta 1: " + qry.lastError().text());
        return;
    }

    // 3. SEGUNDA TENTATIVA: APENAS SC6 (Se a primeira retornar vazio)
    if (!qry.next()) {
        qry.prepare("SELECT C6_NUM, C6_ITEM, C6_PRODUTO, C6_DESCRI, "
                    "C6_QTDVEN, C6_PRUNIT, C6_VALOR, C6_DATFAT, C6_NOTA, "
                    "'' AS C9_LOTECTL, '' AS C9_CLIENTE, '' AS C9_LOJA "
                    "FROM SC6010 WHERE C6_NUM = :pedido AND D_E_L_E_T_ = ' ' "
                    "ORDER BY C6_ITEM");
        qry.bindValue(":pedido", QString::number(wpedido_itens));

        if (!qry.exec() || !qry.next()) {
            return;
        }
    }

    // 4. PREENCHIMENTO DOS DADOS
    int row = 0;
    QSqlQuery qrycc(ProtheusDB);
    qrycc.prepare("SELECT A7_CODCLI FROM SA7010 "
                  "WHERE A7_CLIENTE = :cli AND A7_LOJA = :loja AND A7_PRODUTO = :prod "
                  "AND D_E_L_E_T_ = ' '");
    double total_ped = 0;
    int qtde_produtos = 0;
    do {
        ui->tw_pedido_itens->insertRow(row);

        // Dados Básicos
        ui->tw_pedido_itens->setItem(row, 0, new QTableWidgetItem(qry.value(0).toString().trimmed()));
        ui->tw_pedido_itens->setItem(row, 1, new QTableWidgetItem(qry.value(1).toString().trimmed()));
        ui->tw_pedido_itens->setItem(row, 2, new QTableWidgetItem(qry.value(2).toString().trimmed()));
        ui->tw_pedido_itens->setItem(row, 4, new QTableWidgetItem(qry.value(3).toString().trimmed()));

        // Código no Cliente (SA7)
        QString codCli = "";
        qrycc.bindValue(":cli", qry.value(10).toString());
        qrycc.bindValue(":loja", qry.value(11).toString());
        qrycc.bindValue(":prod", qry.value(2).toString());
        if (qrycc.exec() && qrycc.next()) {
            codCli = qrycc.value(0).toString().trimmed();
        }
        ui->tw_pedido_itens->setItem(row, 3, new QTableWidgetItem(codCli));

        // Valores Numéricos
        double qtde = qry.value(4).toDouble();
        double prec = qry.value(5).toDouble();

        auto *itQtde = new QTableWidgetItem(brasilLocale.toString(qtde, 'f', 0));
        auto *itPrec = new QTableWidgetItem(brasilLocale.toString(prec, 'f', 2));
        auto *itTotal = new QTableWidgetItem(brasilLocale.toString(qtde * prec, 'f', 2));

        itQtde->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        itPrec->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        itTotal->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        ui->tw_pedido_itens->setItem(row, 5, itQtde);
        ui->tw_pedido_itens->setItem(row, 6, itPrec);
        ui->tw_pedido_itens->setItem(row, 7, itTotal);

        // Data (YYYYMMDD para DD/MM/YYYY)
        QString rawData = qry.value(7).toString();
        if (rawData.length() == 8) {
            ui->tw_pedido_itens->setItem(row, 8, new QTableWidgetItem(
                                                     rawData.mid(6, 2) + "/" + rawData.mid(4, 2) + "/" + rawData.left(4)));
        }

        ui->tw_pedido_itens->setItem(row, 9, new QTableWidgetItem(qry.value(8).toString().trimmed()));
        ui->tw_pedido_itens->setItem(row, 10, new QTableWidgetItem(qry.value(9).toString().trimmed()));

        ui->tw_pedido_itens->setRowHeight(row, 25);
        row++;
        total_ped += (qtde * prec) ;
        qtde_produtos += qtde;
    } while (qry.next());

    // 5. AJUSTE DE COLUNAS
    QHeaderView *header = ui->tw_pedido_itens->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Interactive);
    ui->tw_pedido_itens->setColumnWidth(0, 65);  // Pedido
    ui->tw_pedido_itens->setColumnWidth(1, 40);  // Item
    ui->tw_pedido_itens->setColumnWidth(2, 130); // Código Produto
    ui->tw_pedido_itens->setColumnWidth(3, 130); // Código no Cliente
    ui->tw_pedido_itens->setColumnWidth(4, 340); // Descrição
    ui->tw_pedido_itens->setColumnWidth(5, 85);  // Quantidade
    ui->tw_pedido_itens->setColumnWidth(6, 85);  // Preço Unitário
    ui->tw_pedido_itens->setColumnWidth(7, 95);  // Valor Total
    ui->tw_pedido_itens->setColumnWidth(8, 70);  // Data
    ui->tw_pedido_itens->setColumnWidth(9, 70);  // NF
    ui->tw_pedido_itens->setColumnWidth(10, 90); // Lote

    // Faz a coluna de Descrição (índice 4) ocupar o espaço restante se a janela crescer
    ui->tw_pedido_itens->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    ui->txt_qtde_itens->setText(QString::number(row));
    ui->txt_qtde_produtos->setText(QString::number(qtde_produtos));
    ui->txt_total_val->setText(brasilLocale.toString(total_ped, 'f', 2));


}

pedido_itens::~pedido_itens()
{
    delete ui;
}

void pedido_itens::on_bt_voltar_clicked()
{
    close();
}

