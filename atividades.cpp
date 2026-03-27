#include "atividades.h"
#include "ui_atividades.h"


atividades::atividades(SessaoUsuario sessaoA, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::atividades)
{
    ui->setupUi(this);


    showMaximized();

    this->dados_cliente = sessaoA;
    m_dbaModel = new QSqlQueryModel(this);

    QString vendedor = dados_cliente.vendedor.trimmed();
    QSqlDatabase InfokapDB = QSqlDatabase::database("dbi");
    QSqlQuery qry_atividades(InfokapDB);

    // 1. Prepara a query de forma limpa
    QString sql = "SELECT Cliente, LojaCli, NomeCli, Vendedor, UF, PaisDeOrigem, Atividade FROM ClienteAtividade";
    if(vendedor != "000001"){
        sql += " WHERE Vendedor = :vend";
        qry_atividades.prepare(sql);
        qry_atividades.bindValue(":vend", vendedor);
    } else {
        qry_atividades.prepare(sql);
    }

    if(!qry_atividades.exec()){
        QMessageBox::warning(this, "Falha", "Erro ao buscar atividades.");
        return;
    }

    // 2. Alimenta o modelo PRIMEIRO (Sem o while next)
    m_dbaModel->setQuery(qry_atividades);

    // 3. Define os Headers usando o modelo (mais seguro)
    m_dbaModel->setHeaderData(0, Qt::Horizontal, "Cliente");
    m_dbaModel->setHeaderData(1, Qt::Horizontal, "Loja");
    m_dbaModel->setHeaderData(2, Qt::Horizontal, "Nome");
    m_dbaModel->setHeaderData(3, Qt::Horizontal, "Vendedor");
    m_dbaModel->setHeaderData(4, Qt::Horizontal, "UF");
    m_dbaModel->setHeaderData(5, Qt::Horizontal, "Origem");
    m_dbaModel->setHeaderData(6, Qt::Horizontal, "Atividade");

    // 4. Configura a View
    ui->tv_atividades->setModel(m_dbaModel);

    // Comportamento visual
    ui->tv_atividades->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tv_atividades->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tv_atividades->verticalHeader()->hide();
    ui->tv_atividades->setSortingEnabled(true);

    // 5. Distribuição inteligente das colunas (Auto-ajuste)
    QHeaderView *header = ui->tv_atividades->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Interactive); // Permite usuário ajustar
    header->setSectionResizeMode(6, QHeaderView::Stretch);   // Coluna NOME expande para ocupar o resto

    // Ajuste manual de proporção se preferir:
    ui->tv_atividades->setColumnWidth(0, 100);
    ui->tv_atividades->setColumnWidth(1, 50);

    // 6. Atualiza contador (rowCount do modelo é mais confiável)
    // Nota: QSqlQueryModel pode precisar de fetchMore() se a lista for gigantesca
    while (m_dbaModel->canFetchMore()) {
        m_dbaModel->fetchMore();
    }
    ui->lb_qtde->setText(QString::number(m_dbaModel->rowCount()));



}

atividades::~atividades()
{
    delete ui;
}

void atividades::on_bt_voltar_clicked()
{
    close();
}


void atividades::on_bt_pesquisarAtividade_clicked()
{


    QString watividade = ui->txt_p_atividade->text().trimmed();
    QString vendedor = dados_cliente.vendedor;

    QSqlDatabase InfokapDB = QSqlDatabase::database("dbi");
    QSqlQuery qry_atividadesf(InfokapDB);

    // 1. Construção da Query de forma robusta
    QString sql = "SELECT Cliente, LojaCli, NomeCli, Vendedor, "
                  "UF, PaisDeOrigem, Atividade "
                  "FROM ClienteAtividade "
                  "WHERE Atividade LIKE :atividade";

    // Se não for o administrador, filtra pela UF do vendedor (conforme seu código)
    if(vendedor != "000001"){
        sql += " AND UF = :uf";
    }

    qry_atividadesf.prepare(sql);
    qry_atividadesf.bindValue(":atividade", "%" + watividade + "%");

    if(vendedor != "000001"){
        qry_atividadesf.bindValue(":uf", dados_cliente.uf);
    }

    // 2. Execução e validação
    if(!qry_atividadesf.exec()){
        QMessageBox::warning(this, "Falha", "Erro na consulta: " + qry_atividadesf.lastError().text());
        return;
    }

    // 3. Atualiza o modelo existente
    m_dbaModel->setQuery(qry_atividadesf);

    // IMPORTANTE: Força o Qt a ler todos os registros para o contador bater
    while (m_dbaModel->canFetchMore()) {
        m_dbaModel->fetchMore();
    }

    // 4. Atualiza a interface
    int totalVisivel = m_dbaModel->rowCount();
    ui->lb_qtde->setText(QString::number(totalVisivel));

    // Se o modelo perdeu os nomes das colunas ao trocar a query,
    // você pode chamar uma função pequena aqui para renomear os Headers.
   // ajustarHeaders();

    /*
    // if (!m_dbaModel) {
    //     m_dbaModel = new QSqlQueryModel(this);
    //     m_proxyModel = new QSortFilterProxyModel(this);
    //     m_proxyModel->setSourceModel(m_dbaModel);
    //     ui->tv_atividades->setModel(m_proxyModel); // A View sempre olha para o Proxy
    // }

    // m_dbaModel = new QSqlQueryModel(this);
    // m_proxyModel = new QSortFilterProxyModel(this);

    // m_proxyModel->setSourceModel(m_dbaModel);
    // ui->tv_atividades->setModel(m_proxyModel);



    QString watividade = ui->txt_p_atividade->text().trimmed();
    QString vendedor = dados_cliente.vendedor;


    QSqlDatabase InfokapDB = QSqlDatabase::database("dbi");
    QSqlQuery qry_atividadesf(InfokapDB);
    if(vendedor=="000001"){
        qry_atividadesf.prepare("SELECT Cliente, "
                                "LojaCli, NomeCli, "
                                "Propaganda, Multinacional, PaisDeOrigem, Atividade "
                                "FROM ClienteAtividade WHERE Atividade  LIKE :atividade");
    }else{
        qry_atividadesf.prepare("SELECT Cliente, "
                                "LojaCli, NomeCli, "
                                "Propaganda, Multinacional, PaisDeOrigem, Atividade "
                                "FROM ClienteAtividade WHERE Atividade LIKE :atividade "
                                "AND UF= :uf");
         qry_atividadesf.bindValue(":uf",dados_cliente.uf);
    }
    qry_atividadesf.bindValue(":atividade","%"+watividade+"%");

    if(!qry_atividadesf.exec()){
        QMessageBox::warning(this,"Falha","Falha na abertura da consulta");
        return;
    }

    m_dbaModel->setQuery(qry_atividadesf);
    ui->tv_atividades->setModel(m_dbaModel);

    int totalVisivel = m_dbaModel->rowCount();
    ui->lb_qtde->setText(QString::number(totalVisivel));
    */

}


void atividades::on_txt_p_atividade_textChanged(const QString &watividade)
{
    // if (m_proxyModel) {
    //     m_proxyModel->setFilterFixedString(watividade);
    // }

    // Filtra os dados que já estão na tabela instantaneamente
    // Aceita Expressões Regulares (Regex) para buscas avançadas
    //QRegularExpression regExp(watividade, QRegularExpression::CaseInsensitiveOption);
    // m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    // m_proxyModel->setFilterKeyColumn(-1); // -1 filtra em todas as colunas simultaneamente

    //m_proxyModel->setFilterRegularExpression(regExp);
}
