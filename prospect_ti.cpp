#include "prospect_ti.h"
#include "ui_prospect_ti.h"

#include "orcamento_ti.h"
#include "prospect_novo.h"
#include "prospect_editar.h"
#include "struct_comum.h"

QString e_cnpj;
QString e_fantasia;
QString p_limpar_filtro = "N";

prospect_ti::prospect_ti(SessaoUsuario sessao, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::prospect_ti)
    , m_dados(sessao) // Copia o pacote recebido para a variável m_dados
{
    ui->setupUi(this);

    this->showFullScreen();

    atualizar();
}

prospect_ti::~prospect_ti()
{
    delete ui;
}

void prospect_ti::on_bt_voltar_clicked()
{
    close();
}

void prospect_ti::atualizar()
{
    QString uf_vendedor = m_dados.uf;
    QString vendedor = m_dados.vendedor;

    //ui->txt_id->setVisible(false);
    ui->tw_prospects->setRowCount(0);

    QSqlDatabase InfokapDB = QSqlDatabase::database("dbi");

    QSqlQuery qryPrct_vc(InfokapDB);
    qryPrct_vc.prepare("DELETE Prospects FROM Prospects "
                       "JOIN Cotacoes ON Prospects.US_CGC = Cotacoes.CNPJ "
                       "WHERE Cotacoes.CNPJ != ' ' AND Cotacoes.Cliente != '000001'");

    if(!qryPrct_vc.exec()){
        QMessageBox::warning(this, "FALHA", "Falha na exclusão de prospects que viraram clientes.");
        return;
    }

    QSqlQuery qryPrct(InfokapDB);
    QString sql = "SELECT CJ_PROSPE, CJ_LOJPRO, US_NOME, US_NREDUZ, Contato, US_TIPO, "
                  "US_END, US_BAIRRO, US_MUN, US_EST, US_DDD, US_TEL, US_EMAIL, "
                  "US_CGC, Data, Celular, CPF FROM Prospects ";

    QString where = "";
    QString order = " ORDER BY CJ_PROSPE DESC";
    bool useTop = false;

    if(p_limpar_filtro == "N"){
        if(vendedor == "000001"){
            if(!e_cnpj.isEmpty()){
                where = "WHERE US_CGC = :cnpj";
            } else if(!e_fantasia.isEmpty()){
                where = "WHERE US_NREDUZ LIKE :fantasia";
            } else {
                useTop = true;
            }
        } else {
            if(!e_cnpj.isEmpty()){
                where = "WHERE US_CGC = :cnpj AND US_EST = :uf";
            } else if(!e_fantasia.isEmpty()){
                where = "WHERE US_NREDUZ LIKE :fantasia AND US_EST = :uf";
            } else {
                useTop = true;
                where = "WHERE US_EST = :uf";
            }
        }
    } else if (vendedor != "000001"){
        where = "WHERE US_EST = :uf";
    }

    if(useTop) sql.replace("SELECT", "SELECT TOP 400");

    qryPrct.prepare(sql + where + order);

    if(where.contains(":cnpj")) qryPrct.bindValue(":cnpj", e_cnpj);
    if(where.contains(":fantasia")) qryPrct.bindValue(":fantasia", "%" + e_fantasia + "%");
    if(where.contains(":uf")) qryPrct.bindValue(":uf", uf_vendedor);

    if(!qryPrct.exec()){
        QMessageBox::warning(this, "FALHA", "Erro ao buscar prospects.");
        return;
    }

    ui->tw_prospects->setColumnCount(16);
    int cont = 0;
    while (qryPrct.next()) {
        ui->tw_prospects->insertRow(cont);
        for(int i = 0; i < 13; ++i){
            ui->tw_prospects->setItem(cont, i, new QTableWidgetItem(qryPrct.value(i).toString()));
        }

        QString cnpj = qryPrct.value(13).toString().trimmed();
        QString cpf = qryPrct.value(16).toString().trimmed();
        ui->tw_prospects->setItem(cont, 13, new QTableWidgetItem(cnpj.isEmpty() ? cpf : cnpj));

        ui->tw_prospects->setItem(cont, 14, new QTableWidgetItem(qryPrct.value(14).toString()));
        ui->tw_prospects->setItem(cont, 15, new QTableWidgetItem(qryPrct.value(15).toString()));

        ui->tw_prospects->setRowHeight(cont, 30);
        cont++;
    }

    QList<int> widths = {55, 12, 150, 120, 55, 13, 120, 120, 90, 5, 20, 70, 100, 120, 120, 80};
    for(int i = 0; i < widths.size(); ++i) ui->tw_prospects->setColumnWidth(i, widths[i]);

    QStringList cabecalhos = {"N°", "Loja", "Nome", "Fantasia", "Contato", "Tipo", "Endereço", "Bairro", "Municipio", "UF", "DDD", "Telefone", "e-mail 1", "CNPJ/CPF", "Data", "Celular"};
    ui->tw_prospects->setHorizontalHeaderLabels(cabecalhos);
    ui->tw_prospects->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tw_prospects->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tw_prospects->setAlternatingRowColors(true);
    ui->tw_prospects->verticalHeader()->setVisible(false);
    ui->lb_qtde->setText(QString::number(cont));


    // Estilo CSS
    ui->tw_prospects->setStyleSheet(
        "QTableWidget { background-color: white; }"
        "QTableWidget::item { padding: 2px; }"
        "QHeaderView::section { background-color: #f0f0f0; font-weight: bold; }"
        );

    e_cnpj.clear();
    e_fantasia.clear();
    p_limpar_filtro = "N";
}

void prospect_ti::on_bt_pesquisar_clicked()
{
    e_fantasia = QInputDialog::getText(this,"FILTRO","Qual o nome fantasia?").toUpper().trimmed();
    atualizar();
}


void prospect_ti::on_bt_pesquisar_cnpj_clicked()
{
    e_cnpj = QInputDialog::getText(this,"FILTRO","Qual o CNPJ?");
    atualizar();
}


void prospect_ti::on_bt_limpar_filtro_clicked()
{
    p_limpar_filtro = "S";
    atualizar();
}


void prospect_ti::on_bt_novo_clicked()
{
    SessaoUsuario atual;

    atual.uf       = m_dados.uf;

    prospect_novo  *tela  = new prospect_novo(atual, this);
    tela->exec();
}


void prospect_ti::on_bt_editar_clicked()
{
    SessaoUsuario atual;

    int linhaO=ui->tw_prospects->currentRow();
    int id_p_e=ui->tw_prospects->item(linhaO,0)->text().toInt();

    atual.id    = id_p_e;

    prospect_editar  *tela  = new prospect_editar(atual, this);
    tela->exec();

}



void prospect_ti::on_bt_limpar_filtro_2_clicked()
{
    atualizar();
}

