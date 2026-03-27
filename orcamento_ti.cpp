#include "orcamento_ti.h"
#include "ui_orcamento_ti.h"
#include "orcamento_novo.h"
#include "atividades.h"
#include "prospect_ti.h"
#include "pedidos.h"
#include "orcamento_editar.h"



QString e_PDF_si;
QString e_senha;
QString e_sigla;
QString e_vendedor;
QString e_uf_vendedor;
QString e_regiao;
QString e_outlook;
QString e_planilha;

QString name;
QString UsuarioRede;
QString e_Email_filtro;
QString e_nomecli_filtro;
QString e_prospect_filtro;
QString e_codcli_filtro;
QString e_sigla_filtro;
QString e_regiao_filtro;
QString e_limpar_filtro = "N";
QString e_uf_filtro;

QString e_todas = "150";
int e_id = 0;

QString cod_nova_transp;  // para quando o usuário escolher uma transportadora não cadastrada.



 orcamento_ti::orcamento_ti(QWidget *parent)
     : QMainWindow(parent)
     , ui(new Ui::orcamento_ti)
     , dbp (QSqlDatabase::addDatabase("QODBC","dbp"))
     , dbi (QSqlDatabase::addDatabase("QODBC","dbi"))
     , dba (QSqlDatabase::addDatabase("QODBC","dba"))  //permite alterar as tabelas do Protheus
     , m_dbpModel(new QSqlTableModel(this,dbp))
     , m_dbiModel(new QSqlTableModel(this,dbi))
     , m_dbaModel(new QSqlTableModel(this,dba))
{
    ui->setupUi(this);

    // ui->progressBar->setVisible(false);
    // ui->lb_vp->setVisible(false);
    ui->txt_msg->setVisible(false);
    ui->tw_excel->setVisible(false);
    ui->txt_cod_transp->setVisible(false);

    QString ipkap = identificarIPs().left(11);

    this->setFocus();

    QString local="C:";
    QString nome="/Cotacoes";
    QDir dir(local+nome);
    if(dir.exists()){
        //QMessageBox::about(this,"pasta","Diretório já existe......");    // não apagar
    }else{
        dir.mkdir(local+nome);
    }




    if(ipkap == "192.168.247"){
        dbp.setHostName("192.168.247.198");
        dbp.setDatabaseName("consultaaccess");
        dbp.setUserName("leitura");
        dbp.setPassword("leitura");

        if(!dbp.open()){
            QMessageBox::warning(this,"ERRO","Não conectado ao banco de dados Protheus");
            return;
        }else{
            // QMessageBox::information(this,"AVISO","Conectado ao banco de dados Protheus");
        }

        dbi.setHostName("192.168.247.198");
        dbi.setDatabaseName("INFOKAP");
        dbi.setUserName("moreschi");
        dbi.setPassword("kap1954@");


        if(!dbi.open()){
            QMessageBox::warning(this,"ERRO","Não conectado ao banco de dados Infokap");
            return;
        }else{
            //QMessageBox::information(this,"AVISO","Conectado ao banco de dados Infokap");
        }

        dba.setHostName("192.168.247.198");
        dba.setDatabaseName("P12_Oficial");
        dba.setUserName("totvs_odbc");
        dba.setPassword("totvs_odbc");
        //QSqlDatabase ProtheusDBa = QSqlDatabase::database("dba");

    }else{

        dbp.setHostName("kapbr.ddns.net");
        dbp.setPort(1433);

        //Tentativa de conexão para o Protheus leitura
        if (!abrirConexao(dbp, "kapbr.ddns.net", "P12DB", "leitura", "leitura")) {
            QMessageBox::warning(this, "ERRO", "Falha ao conectar no Protheus:\n" + dbp.lastError().text());
            return;
        }

        dba.setHostName("kapbr.ddns.net");
        dba.setPort(1433);
        //Tentativa de conexão para o Protheus gravação
        if (!abrirConexao(dba, "kapbr.ddns.net", "P12DB", "totvs_odbc", "totvs_odbc")) {
            QMessageBox::warning(this, "ERRO", "Falha ao conectar no Protheus - totvs_odbc:\n" + dbp.lastError().text());
            return;
        }

        dbi.setHostName("kapbr.ddns.net");
        dbi.setPort(1433);

        // Tentativa de conexão para o Infokap
        if (!abrirConexao(dbi, "kapbr.ddns.net", "Infokap", "user_infokap", "Lp9!DNTx37-F+8D_1z_2")) {
        //if (!abrirConexao(dbi, "kapbr.ddns.net", "Infokap", "moreschi", "kap1954@")) {
            QMessageBox::warning(this, "ERRO", "Falha ao conectar no Infokap:\n" + dbi.lastError().text());
            return;
        }
    }


    // verificando o usuário do computador
    name = qgetenv("USER");
    if (name.isEmpty()){
        name = qgetenv("USERNAME");
        UsuarioRede = name;
    }



    tv_transportadoras = new QTableView(this);
    // tv_transportadoras->setStyleSheet("background-color: white; border: 2px solid gray; selection-background-color: red;");
    tv_transportadoras ->setStyleSheet("background-color: rgb(100,255,255); selection-background-color: blue;");
    tv_transportadoras->setWindowFlags(Qt::SubWindow);
    tv_transportadoras->setVisible(false);



    QSqlDatabase InfokapDB = QSqlDatabase::database("dbi");
    QSqlQuery buscar_sigla(InfokapDB);
    buscar_sigla.prepare("SELECT Sigla, Apelido, Vendedor, UF, Regiao, NivelAcesso, Outlook, Planilha "
                         "FROM Funcionarios2 WHERE Ativo='S' AND usuario = :apelido");
    buscar_sigla.bindValue(":apelido",name);
    if(!buscar_sigla.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta buscar_sigla "+buscar_sigla.lastError().text());
        return;
    }
    buscar_sigla.first();
    e_sigla = buscar_sigla.value(0).toString();
    e_vendedor = buscar_sigla.value(2).toString().trimmed();
    e_uf_vendedor = buscar_sigla.value(3).toString().toUpper();
    e_regiao = buscar_sigla.value(4).toString().toUpper();
    e_outlook = buscar_sigla.value(6).toString().toUpper();
    e_planilha = buscar_sigla.value(7).toString().trimmed();


    if(e_sigla.isNull()){
        QString u_versao = "20241024";
        QString sigla = QInputDialog::getText(this,"Orçamento-Login ","   Qual a sua sigla? ");
        QString senha = QInputDialog::getText(this,"Orçamento-Login ","   Informe a sua senha:   ",QLineEdit::Password);
        e_sigla = sigla.toUpper();
        e_senha = senha;

        QSqlQuery qry_sigla(InfokapDB);
        qry_sigla.prepare("SELECT Sigla, Senha, Vendedor, Ativo, UF, Regiao, NivelAcesso, Outlook, Planilha "
                          "FROM Funcionarios2 "
                          "WHERE Sigla = :sigla "
                          "AND Senha = :senha "
                          "AND Ativo = 'S'");
        qry_sigla.bindValue(":sigla",sigla);
        qry_sigla.bindValue(":senha",senha);
        if(!qry_sigla.exec()){
            QMessageBox::warning(this,"FALHA","Falha na consulta qry_sigla "+qry_sigla.lastError().text());
            return;
        }
        qry_sigla.first();
        e_vendedor = qry_sigla.value(2).toString().trimmed();
        e_uf_vendedor = qry_sigla.value(4).toString().toUpper();
        e_regiao = qry_sigla.value(5).toString().toUpper();
        e_outlook = qry_sigla.value(7).toString().toUpper();
        e_planilha = qry_sigla.value(8).toString().trimmed();

        if(qry_sigla.value(0).toString()==""){
            QMessageBox::information(this,"Senha","Senha e/ou sigla incorretas");
            InfokapDB.close();
            orcamento_ti::closeEvent(0);
            return  ;
        }
    }

    ui->statusbar->showMessage("F9 - Pedidos     F8 - Estoque     Usuário: "+UsuarioRede);

    atualizarTi();

}

orcamento_ti::~orcamento_ti()
{
    delete ui;
}


QString orcamento_ti::identificarIPs() {
    // Retorna uma lista de todos os endereços IP encontrados na máquina
    const QList<QHostAddress> listaEnderecos = QNetworkInterface::allAddresses();

    for (const QHostAddress &endereco : listaEnderecos) {
        // Filtramos para pegar apenas IPv4 e ignorar o 127.0.0.1
        if (endereco.protocol() == QAbstractSocket::IPv4Protocol && !endereco.isLoopback()) {
            //qDebug() << "IP Encontrado:" << endereco.toString();
            return endereco.toString();
        }
    }
}


bool orcamento_ti::abrirConexao(QSqlDatabase &db, QString host, QString dbName, QString user, QString pass) {

    // String de conexão direta para o Driver 17
    QString connString = QString("DRIVER={ODBC Driver 17 for SQL Server};"
                                 "SERVER=%1;"
                                 "DATABASE={%2};"
                                 "UID=%3;"
                                 "PWD={%4};")
                             .arg(host, dbName, user, pass);

    db.setDatabaseName(connString);

    if (db.open()) {
        return true;
    } else {
        // Opcional: imprimir o erro no console para debug
        qDebug() << "Erro de conexão:" << db.lastError().text();
        return false;
    }
}

void orcamento_ti::on_bt_sair_clicked()
{
    close();
}

void orcamento_ti::atualizarTi()
{

        ui->tw_orcamentos->setRowCount(0); // Mais rápido que o while manual
        ui->tw_orcamentos->setSortingEnabled(false); // Importante para performance

        QSqlDatabase InfokapDB = QSqlDatabase::database("dbi");
        QLocale brasilLocale(QLocale::Portuguese, QLocale::Brazil);
        QSqlQuery qry(InfokapDB);

        // 1. CONSTRUÇÃO DA QUERY COM BIND VALUES (Segurança)
        QString sql = "SELECT ";
        if(e_limpar_filtro == "N") sql += "TOP " + e_todas + " ";

        sql += "NDaCotacao, Cliente, A1_LOJA, A1_TIPO, Fantasia, PROSP, Data, Atendente, "
               "telefone, ValorTot, Contato, email, CNPJ, UF, NaoEnviar, TP, Deletado, CPF, Regiao "
               "FROM Cotacoes WHERE Deletado IS NULL ";

        if(e_limpar_filtro == "N") {

            if(!e_sigla_filtro.isEmpty())           sql += " AND Atendente = :atendente ";
            if(!e_codcli_filtro.isEmpty())          sql += " AND Cliente = :cliente ";
            if(!e_nomecli_filtro.isEmpty())       sql += " AND Fantasia LIKE :fantasia ";
            if(!e_uf_filtro.isEmpty())               sql += " AND UF = :uf_filtro ";
            if(!e_prospect_filtro.isEmpty())      sql += " AND PROSP = :prospect ";
            if(!e_Email_filtro.isEmpty())          sql += " AND email LIKE :email ";
            if(!e_regiao_filtro.isEmpty())         sql += " AND Regiao = :reg_filtro ";

            if(e_vendedor != "000001" && e_uf_vendedor == "RS" && e_vendedor == "0063") {
                sql += "AND uf = :uf ";
            } else if(e_vendedor != "000001") {
                sql += "AND Regiao = :regiao ";
            }
        }
        sql += "ORDER BY NDaCotacao DESC";

        qry.prepare(sql);

        if(sql.contains(":atendente"))  qry.bindValue(":atendente",  e_sigla_filtro );
        if(sql.contains(":cliente"))    qry.bindValue(":cliente", e_codcli_filtro);
        if(sql.contains(":fantasia"))   qry.bindValue(":fantasia", "%" + e_nomecli_filtro + "%");
        if(sql.contains(":uf_filtro"))  qry.bindValue(":uf_filtro", e_uf_filtro);
        if(sql.contains(":prospect"))   qry.bindValue(":prospect", e_prospect_filtro);
        if(sql.contains(":email"))      qry.bindValue(":email", "%" + e_Email_filtro + "%");
        if(sql.contains(":reg_filtro")) qry.bindValue(":reg_filtro", e_regiao_filtro);

        if(sql.contains(":uf")) qry.bindValue(":uf", e_uf_vendedor);
        if(sql.contains(":regiao")) qry.bindValue(":regiao", e_regiao);

        if(!qry.exec()){
            QMessageBox::warning(this,"FALHA","Erro no banco: " + qry.lastError().text());
            return;
        }

        // 2. CONFIGURAÇÃO PRECISA DA PROGRESS BAR
        // Em vez de fazer uma segunda query (que é lento), usamos o size()
        // ou contamos os registros após o exec().
        int totalRegistros = 0;
        if (InfokapDB.driver()->hasFeature(QSqlDriver::QuerySize)) {
            totalRegistros = qry.size();
        } else {
            // Se o driver não suportar size(), usamos o recordCount da consulta
            qry.last();
            totalRegistros = qry.at() + 1;
            qry.first();
            qry.previous(); // Volta para antes do primeiro para o while(next)
        }

        // ui->progressBar->setVisible(true);
        // ui->progressBar->setRange(0, totalRegistros > 0 ? totalRegistros : 100);
        // ui->progressBar->setValue(0);

        // 3. PREENCHIMENTO DA TABELA
        int cont = 0;
        ui->tw_orcamentos->setSortingEnabled(false); // Desativa sorting durante inserção (Ganho de performance)
        ui->tw_orcamentos->setColumnCount(16);

        while (qry.next()) {
            ui->tw_orcamentos->insertRow(cont);

            // Dados básicos
            for(int i=0; i<=5; ++i)
                ui->tw_orcamentos->setItem(cont, i, new QTableWidgetItem(qry.value(i).toString().toUpper()));

            // Cor cinza para enviados
            if(qry.value(15).toString() == "S"){
                ui->tw_orcamentos->item(cont,0)->setForeground(Qt::gray);
            }

            // Data formatada de forma simplificada
            QDate dataDb = qry.value(6).toDate();
            ui->tw_orcamentos->setItem(cont, 6, new QTableWidgetItem(dataDb.toString("dd/MM/yyyy")));

            ui->tw_orcamentos->setItem(cont, 7, new QTableWidgetItem(qry.value(7).toString()));
            ui->tw_orcamentos->setItem(cont, 8, new QTableWidgetItem(qry.value(8).toString()));

            // Moeda (ValorTot)
            double valor = qry.value(9).toDouble();
            QTableWidgetItem *itemValor = new QTableWidgetItem(brasilLocale.toString(valor, 'f', 2));
            itemValor->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            ui->tw_orcamentos->setItem(cont, 9, itemValor);

            ui->tw_orcamentos->setItem(cont, 10, new QTableWidgetItem(qry.value(10).toString()));
            ui->tw_orcamentos->setItem(cont, 11, new QTableWidgetItem(qry.value(11).toString()));
            ui->tw_orcamentos->setItem(cont, 12, new QTableWidgetItem(qry.value(12).toString()));

            // UF com destaque para NaoEnviar
            QTableWidgetItem *itemUf = new QTableWidgetItem(qry.value(13).toString());
            if(qry.value(14).toString() == "1"){
                itemUf->setBackground(Qt::red);
            }
            ui->tw_orcamentos->setItem(cont, 13, itemUf);

            ui->tw_orcamentos->setItem(cont, 14, new QTableWidgetItem(qry.value(18).toString())); // Regiao
            ui->tw_orcamentos->setItem(cont, 15, new QTableWidgetItem(qry.value(14).toString())); // NaoEnviar

            ui->tw_orcamentos->setRowHeight(cont, 30); // Altura mínima legível

            cont++;
            //ui->progressBar->setValue(cont); // O Qt cuida da porcentagem sozinho
            QCoreApplication::processEvents(); // Mantém a interface responsiva
        }

        configurarTabela();

        // 4. ESTILIZAÇÃO FINAL
     //   configurarCabecalho(); // Dica: coloque o setup de colunas em uma função separada
        ui->tw_orcamentos->setSortingEnabled(true);
        ui->lb_qtde->setText(QString::number(cont));
        e_todas = "150";

}


void orcamento_ti::configurarTabela()
{
    // Definição de Larguras das Colunas
    QMap<int, int> larguras = {
        {0, 55}, {1, 55}, {2, 10}, {3, 10}, {4, 170}, {5, 150},
        {6, 75}, {7, 40}, {8, 90}, {9, 90}, {10, 140}, {11, 240},
        {12, 100}, {13, 30}, {14, 45}, {15, 10}
    };

    for (auto it = larguras.begin(); it != larguras.end(); ++it) {
        ui->tw_orcamentos->setColumnWidth(it.key(), it.value());
    }

    // Títulos das Colunas
    QStringList cabecalhos = {
        "N°", "Cliente", "Loja", "T.", "Nome", "Prospect",
        "Data", "Kap", "Telefone", "Valor R$", "Contato",
        "e-mail", "CNPJ", "UF", "Região", "Não"
    };
    ui->tw_orcamentos->setHorizontalHeaderLabels(cabecalhos);

    // Comportamento e Estilo
    ui->tw_orcamentos->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tw_orcamentos->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tw_orcamentos->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tw_orcamentos->verticalHeader()->setVisible(false);
    ui->tw_orcamentos->setAlternatingRowColors(true);
    ui->tw_orcamentos->horizontalHeader()->setStretchLastSection(true);

    // Estilo CSS
    ui->tw_orcamentos->setStyleSheet(R"(
        QTableWidget {
            background-color: #F8FAFC;
            alternate-background-color: #EDF2F7;
            selection-background-color: #DBEAFE;
            selection-color: #1E293B;
            gridline-color: #E2E8F0;
        border: 1px solid #CBD5E0;
            font-size: 9pt;
        outline: none }
)");

}

void orcamento_ti::on_bt_novo_clicked()
{
    SessaoUsuario atual;

    atual.usuario = UsuarioRede;
    atual.sigla  = e_sigla;
    atual.regiao   = e_regiao;
    atual.vendedor = e_vendedor;
    atual.uf       = e_uf_vendedor;

   orcamento_novo  *tela  = new orcamento_novo(atual, this);
    tela->exec();

    atualizarTi();
}


void orcamento_ti::on_bt_visualizarPDF_clicked()
{
    if(ui->tw_orcamentos->currentRow()==-1){
        QMessageBox::warning(this,"AVISO","<h3>Você não informou um orçamento para visualizar o PDF</h3>");
        return;
    }

    QLocale brasilLocale(QLocale::Portuguese,QLocale::Brazil);
    QPrinter printer;

    int linhaO=ui->tw_orcamentos->currentRow();
    int id=ui->tw_orcamentos->item(linhaO,0)->text().toInt();

    QString aux_nomeCli = ui->tw_orcamentos->item(linhaO,4)->text().simplified().toUpper();
    std::replace(aux_nomeCli.begin(),aux_nomeCli.end(),' ','_');
    QString cliente = aux_nomeCli;

    QString aux_nomePros;
    aux_nomePros = ui->tw_orcamentos->item(linhaO,5)->text().simplified().toUpper();
    std::replace(aux_nomePros.begin(),aux_nomePros.end(),' ','_');
    QString prospect = aux_nomePros;

    QString data1=ui->tw_orcamentos->item(linhaO,6)->text().left(2);
    QString data2=ui->tw_orcamentos->item(linhaO,6)->text().mid(3,2);
    QString data3=ui->tw_orcamentos->item(linhaO,6)->text().right(4);
    QString data4="Data_"+ data1 + "_" + data2 + "_" + data3;

    QSqlDatabase infokapDB = QSqlDatabase::database("dbi");
    QSqlQuery qryhorario(infokapDB);
    qryhorario.prepare("SELECT horario, NaoEnviar, NDaCotacao, Cliente, A1_LOJA, CJ_PROSPE, Valor_frete "
                       "FROM Cotacoes "
                       "WHERE NDaCotacao = :id");
    qryhorario.bindValue(":id",QString::number(id));
    if(!qryhorario.exec()){
        QMessageBox::warning(this,"ERRO","ERRO ao abrir horário do orçamento  "+ QString::number(id));
        // return;
    }
    qryhorario.first();
    bool vainaovai = qryhorario.value(1).toBool();
    if(vainaovai!=0){
        QMessageBox::information(this,"AVISO","Este orçamento não pode ser enviado. Altere o status");
        return;
    }

    QString hora = qryhorario.value(0).toString().mid(11,2);
    QString minuto = qryhorario.value(0).toString().mid(14,2);
    QString horario = hora + ":" + minuto;
    QString nome;

    QString cod_cli = qryhorario.value(3).toString();
    QString loj_cli = qryhorario.value(4).toString();
    double val_frete = qryhorario.value(6).toDouble();

    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
    QSqlQuery qryregiao(ProtheusDB);
    qryregiao.prepare("SELECT A1_REGIAO"
                      " FROM SA1010"
                      " WHERE A1_COD = :codcli"
                      " AND A1_LOJA = :lojcli");
    qryregiao.bindValue(":codcli",cod_cli);
    qryregiao.bindValue(":lojcli",loj_cli);
    if(!qryregiao.exec()){
        QMessageBox::warning(this,"FALHA","Falha ao abrir a consulta qryregiao" );
        return;
    }
    qryregiao.first();
    QString regiao = qryregiao.value(0).toString();

    if(cliente == "ORCAMENTO"){
        nome = "Kap_Orcamento_Numero_" + QString::number(id) + "_" + prospect + "_" + data4 + ".pdf";
    }else{
        nome = "Kap_Orcamento_Numero_" + QString::number(id) + "_" + cliente + "_" + data4 + ".pdf";
    }


    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName("C:/Cotacoes/"+nome);
    QPainter painter;
    if(!painter.begin(&printer)){
        QMessageBox::warning(this,"ERRO","Erro ao abrir PDF");
        return;
    }

    if(regiao == "EXT"){
        QPixmap logo ("U:/Biblioteca/Imagens_Gerais/Logos+Usados/kap_top_cor_eng.jpg") ;
        painter.drawPixmap(30,05,QPixmap(logo.scaled(200,300,Qt::KeepAspectRatio)));
    }else{
        if(e_vendedor == "000001"){
            QPixmap logo ("U:/Biblioteca/Imagens_Gerais/Logos+Usados/kap_top_cor.gif") ;
            painter.drawPixmap(30,05,QPixmap(logo.scaled(200,300,Qt::KeepAspectRatio)));
        }else{
            QPixmap logo ("C:/Users/Public/kap_top_cor.gif") ;
            painter.drawPixmap(30,05,QPixmap(logo.scaled(200,300,Qt::KeepAspectRatio)));
        }
    }

    QString digi, impr, orca, endk, clie, celu, ende, aocu, aten, tele, rama, prod, desc, qtde, puni, parc, praz, entr, diau ;
    if(regiao == "EXT"){
        digi = " Typed    in:";
        impr = " Printed  on:";
        orca = "Budget";
        endk = "Rua Carmo do Rio Verde, 78 - Santo Amaro - ZIP: 04729-010  São Paulo - SP -  Telephone: 55 11 5645 4444 -  CNPJ: 61.094.165/0001-64";
        clie = "Client:";
        celu = "Phone:";
        ende = "Address:";
        aocu = "Care of:";
        aten = "Made by:";
        tele = "Phone:";
        rama = "ext:";
        prod = "Product Kap";
        desc = "Description";
        qtde = "Qtdy";
        puni = "Un price";
        parc = "Partial";
        praz = "Delivery";
        entr = "Time";
        diau = "Working days";
    }else{
        digi = " Digitado  em:";
        impr = " Impresso em:";
        orca = "Orçamento";
        endk = "Rua Carmo do Rio Verde, 78 - Santo Amaro - 04729-010  São Paulo - SP -  Fone: (11) 5645 4444 -  CNPJ: 61.094.165/0001-64 - IE: 103401318114";
        clie = "Cliente:";
        celu = "Cel:";
        ende = "Endereço:";
        aocu = "At:";
        aten = "Atendente:";
        tele = "Tel:";
        rama = "ramal:";
        prod = "Código Kap";
        desc = "Descrição";
        qtde = "Qtde";
        puni = "Unitário";
        parc = "Parcial";
        praz = "Prazo P/";
        entr = "Emitir NF.";
        diau = "Dias úteis";
    }
    painter.drawText(320,25,digi);
    painter.drawText(320,40,impr);

    painter.drawText(400,25,data1 + "/" + data2 + "/" + data3 + " " + horario);
    painter.drawText(400,40,QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));

    painter.setFont(QFont("Tahoma", 22));

    painter.drawText(600,25,orca);
    int linhaOrc = ui->tw_orcamentos->currentRow();
    painter.drawText(643,55,QString::number(ui->tw_orcamentos->item(linhaOrc,0)->text().toInt()));

    painter.setFont(QFont("Tahoma", 8));
    painter.drawText(30,75,endk);
    painter.setFont(QFont("Tahoma", 12));

    painter.drawLine(0,80,765,80);

    painter.drawText(1,95,clie);
    painter.drawText(620,95,celu);
    painter.drawText(1,115,ende);
    if(regiao != "EXT"){
        painter.drawText(620,115,"CNPJ:");
    }
    painter.drawText(1,135,aocu);
    painter.drawText(225,135,"E-mail:");
    painter.drawText(620,135,tele);
    painter.drawText(1,155,aten);
    painter.drawText(225,155,"E-mail:");
    painter.drawText(480,155,rama);
    painter.drawText(620,155,celu);
    QPen pen;
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawLine(0,160,780,160);

    //Lendo o cabeçalho do orçameto
    int numOrc = ui->tw_orcamentos->item(linhaOrc,0)->text().toInt();
    QSqlQuery qrycab(infokapDB);
    qrycab.prepare("Select Cliente, A1_LOJA, Fantasia, Cidade, UF, Endereco, "
                   " CelularCli, CNPJ, Contato, Email, Telefone, Atendente, DDL,"
                   " OBS, Desconto, Frete, EnviarFichaCadastral, Semail, CustoFinanceiro, Desconto"
                   " FROM Cotacoes "
                   "where NDaCotacao = :orca");
    qrycab.bindValue(":orca",QString::number(numOrc));

    if (!qrycab.exec()) {
        QMessageBox::warning(this, "FALHA", "Falha na consulta qry (X): " + qrycab.lastError().text());
        return;
    }

    qrycab.first();

    QFont fonte1("Tahoma",8,QFont::Bold,true);
    painter.setFont(fonte1);

    if(cliente == "ORCAMENTO"){
        painter.drawText(100,95,prospect); // fantasia do prospect
    }else{
        painter.drawText(75,95,qrycab.value("Cliente").toString().trimmed());  // Codigo cliente
        painter.drawText(135,95,qrycab.value("A1_LOJA").toString().trimmed()); // loja
        painter.drawText(165,95,qrycab.value("Fantasia").toString().toUpper().trimmed()); // fantasia
    }

    QString cidade = qrycab.value("Cidade").toString().trimmed();
    QString UF = qrycab.value("UF").toString();
    QString endereco = qrycab.value("Endereco").toString().trimmed();
    QString endereco1 = endereco + " " + cidade + " " + UF;

    painter.drawText(670,95,qrycab.value("CelularCli").toString().trimmed());
    painter.drawText(80,115,endereco1);
    painter.drawText(670,115,qrycab.value("CNPJ").toString().trimmed());
    painter.drawText(80,135,qrycab.value("Contato").toString().trimmed());  // At.
    painter.drawText(275,135,qrycab.value("Email").toString().trimmed()); // email
    painter.drawText(670,135,qrycab.value("Telefone").toString().trimmed());

    QString sigla = qrycab.value("Atendente").toString().toUpper();              //Sigla do atendente
    QSqlQuery qrysigla(infokapDB);
    qrysigla.prepare("Select Apelido, Email, Ramal, Celular "
                            "FROM Funcionarios2 "
                            "WHERE Sigla = :sigla");
    qrysigla.bindValue(":sigla",sigla);
    if(!qrysigla.exec()){
        QMessageBox::warning(this,"ERRO","Erro ao abrir tabela de funcionários");
        return;
    }else{
        //QString desenho3D = "<a href = 'https://www.kap.com.br/biblioteca/desenhos-3d/microrutores--micro-chaves-/serie-m3'</a>";
        qrysigla.first();
        painter.drawText(80,155,qrysigla.value(0).toString().trimmed());  // atendente apelido
        painter.drawText(275,155,qrysigla.value(1).toString().trimmed()); // atendente e-mail
        painter.drawText(525,155,qrysigla.value(2).toString().trimmed()); // atendente ramal
        painter.drawText(670,155,qrysigla.value(3).toString().trimmed()); // atendente celular
    }


    //Lendo os itens do orçamento //----------------------------------
    QSqlQuery qryrel(infokapDB);
    qryrel.prepare("SELECT * FROM CotacoesProdutos"
                   " where NDaCotacao = :orca"
                   " AND Imprime =1 ORDER BY nitem");
    qryrel.bindValue(":orca",QString::number(numOrc));
    if(!qryrel.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta qryrel: " + qryrel.lastError().text());
        return;
    }
    qryrel.first();
    painter.setFont(QFont("Tahoma", 9));

    int linha = 210;
    int salto = 140;
    int pagina = 0;
    int qrpp = 870;          //qrpp = Qtde de Registros Por Página

    double totalsub = 0;
    double w_st = 0;
    double v_st = 0;
    double s_st = 0;
    double custoF2 = 0;
    double v_frete = 0;
    double valor_ipi = 0;
    double  s_ipi = 0;
    double custoF = qrycab.value("CustoFinanceiro").toDouble();
    double desconto = qrycab.value("Desconto").toDouble();
    double v_desc = 0;              //valor de cada desconto
    double s_desc = 0;              //soma dos descontos
    double vu_i = 0;                //valor unitário original
    double vu = 0;                  //valor unitário com desconto
    double soma_unitario =  0;      //soma de cada valor unitário
    double custoFunit = 0;          //valor do custo financeiro
    double s_soma_unitario = 0;      //soma do subtotal de cada item

    do{
        totalsub += (qryrel.value(4).toDouble() * qryrel.value(5).toDouble());  // soma de (qtde * vu)
    }while (qryrel.next());
    qryrel.first();

    do{
        painter.setPen(QPen(Qt::blue,1));
        if(pagina==0){
            painter.drawLine(0,linha,780,linha);
            painter.setFont(QFont("Tahoma", 10));
            painter.drawText(01,199,"It.");
            painter.drawText(26,199,prod);
            painter.drawText(220,199,desc);

            painter.setPen(QPen(Qt::red,1));
            painter.drawText(510,175,"I");  //175
            painter.setPen(QPen(Qt::blue,1));

            if(regiao!="EXT"){
                //painter.drawText(512,187,"ST");
                painter.drawText(598,187,"Preço");
                painter.drawText(651,187,"IPI");
            }
            painter.drawText(689,187,"Total");
            painter.setPen(QPen(Qt::red,1));
            painter.drawText(722,175,"*");

            QRectF rectanglePrazo(730, 165, 200.0, 40.0);
            painter.setPen(QPen(QColor("white")));
            painter.setBrush(Qt::cyan);
            painter.drawRect(rectanglePrazo) ;
            painter.setPen(QPen(QColor("blue")));

            painter.setBrush(Qt::white);

            painter.drawText(730,175,praz);
            painter.drawText(459,199,"NCM");

            painter.setPen(QPen(Qt::red,1));
            painter.drawText(510,187,"m");
            painter.setPen(QPen(Qt::blue,1));

            painter.drawText(532,199,qtde); //552
            painter.drawText(593,199,puni);
            if(regiao != "EXT"){
                painter.drawText(651,199,"(%)");
            }
            painter.drawText(685,199,parc);
            painter.setFont(QFont("Tahoma", 8));
            painter.drawText(730,187,entr);
            painter.setFont(QFont("Tahoma", 10));

            painter.setPen(QPen(Qt::red,1));
            painter.drawText(510,199,"p.");
            painter.setPen(QPen(Qt::blue,1));

            painter.setFont(QFont("Tahoma", 8));
            painter.drawText(730,199,diau) ;
            painter.setFont(QFont("Tahoma", 10));

            //painter.drawLine(1,203,780,203);

        }else{
            painter.drawLine(0,linha+2,780,linha+2);

            painter.setFont(QFont("Tahoma", 10));
            painter.drawText(01,54,"It.");
            painter.drawText(26,54,"Código Kap");
            painter.drawText(220,54,"Descrição");

            painter.setPen(QPen(Qt::red,1));
            painter.drawText(510,30,"I");  //30
            painter.setPen(QPen(Qt::blue,1));

            //painter.drawText(517,42,"ST");
            painter.drawText(598,42,"Preço");
            painter.drawText(651,42,"IPI");
            painter.drawText(689,42,"Total");
            painter.setPen(QPen(Qt::red,1));
            painter.drawText(723,30,"*");

            QRectF rectanglePrazo(735, 20, 200.0, 40.0);
            painter.setPen(QPen(QColor("white")));
            painter.drawRect(rectanglePrazo) ;
            painter.setBrush(Qt::cyan);
            painter.setPen(QPen(Qt::blue,1));
            painter.drawText(730,30,praz);
            painter.setBrush(Qt::white);

            painter.drawText(459,54,"NCM");  // 42

            painter.setPen(QPen(Qt::red,1));
            painter.drawText(510,42,"m");
            painter.setPen(QPen(Qt::blue,1));

            //painter.drawText(512,54,"(%)");
            painter.drawText(532,54,"Qtde");  //552
            painter.drawText(593,54,"Unitário");
            painter.drawText(651,54,"(%)");
            painter.drawText(685,54,"Parcial");
            painter.drawText(733,42,"Emitir NF");

            painter.setPen(QPen(Qt::red,1));
            painter.drawText(510,54,"p.");
            painter.setPen(QPen(Qt::blue,1));

            painter.setFont(QFont("Tahoma", 8));
            painter.drawText(730,54,diau);
            painter.setFont(QFont("Tahoma", 10));
            painter.drawLine(01,57,780,57);
        }
        painter.setPen(QPen(Qt::black,1));

        painter.setFont(QFont("Tahoma", 9));
        QString item =(qryrel.value(2).toString());
        linha+=20;

        QString codproduto = qryrel.value(1).toString();
        QString serie = qryrel.value(23).toString().trimmed().toLower();

        QString d0 = "https://www.kap.com.br/biblioteca/desenhos-3d/";
        QString link_d3D;

        if(serie=="m3" || serie == "mk"  || serie == "mx"  || serie == "mm"  || serie == "mmp"  || serie == "mv"  || serie == "mvk"  || serie == "mvp"
            || serie == "mf"  || serie == "mc"  || serie == "mp"){
            link_d3D = d0+"microrutores--micro-chaves-/serie-"+serie;
        }
        if(serie=="lb"  || serie == "le"  || serie == "lc"  || serie == "lq"){
            link_d3D = d0+"interruptores-de-posicao-/serie-"+serie;
        }
        if(serie=="les-hp"  || serie == "les"  || serie == "rs"  || serie == "gs"){
            link_d3D = d0+"dispositivos-de-seguranca-/serie-"+serie;
        }
        if(serie=="sd"  || serie == "sf"  || serie == "sk"  || serie == "se" || serie == "sp"){
            link_d3D = d0+"interruptores-de-pedal-/serie-"+serie;
        }
        if(serie=="c"  || serie == "c9"  || serie == "c5q"  || serie == "tb"){
            link_d3D = d0+"comando-e-sinalizacao-/serie-"+serie;
        }
        if(serie=="pb"  || serie == "pc"  || serie == "pd"  || serie == "ph"  || serie == "pn"){
            link_d3D = d0+"plugues-e-tomadas-/serie-"+serie;
        }

        painter.drawText(01,linha,item);

        // painter.drawText(18,linha,  codproduto   ); // Codprod


        QTextDocument linkDoc;
        linkDoc.setHtml("<p><a href="+link_d3D+">"+codproduto+"</a></p>");
        linkDoc.setTextWidth(180);
        painter.save();
        painter.translate(18, linha - 18);
        linkDoc.drawContents(&painter);
        painter.restore();


        if(e_PDF_si != "1"){
            QString imagem1 = qryrel.value(11).toString().left(7).trimmed();
            if(imagem1.trimmed().isEmpty() ||imagem1.trimmed() == "0"){
                imagem1 = qryrel.value(1).toString().trimmed().toUpper();
                QPixmap imagem2 ("L:/DAT/Imagens_Orcamento/" + imagem1+ ".jpg");
                painter.drawPixmap(26,linha+10,QPixmap(imagem2.scaled(110,138,Qt::KeepAspectRatio)));
            }else{
                QString imagem_prod2=QString(imagem1+".jpg");
                QString imagemWeb = "C:/Users/"+name+"/Kap Componentes Eletricos Ltda/InfoKapQT - Documentos/prodimagem/"+imagem_prod2;
                QPixmap imagem2(imagemWeb);
                //QPixmap imagem2 ("K:/" + imagem1 + ".jpg");
                painter.drawPixmap(26,linha+10,QPixmap(imagem2.scaled(110,138,Qt::KeepAspectRatio)));
            }
        }

        int tcodcli = qryrel.value(14).toString().trimmed().length();
        if(tcodcli > 0){
            painter.setPen(QPen(Qt::gray,1));
            if(e_PDF_si != "1"){
                painter.drawText(05,linha+131,"No cliente:");
                painter.drawText(65,linha+131,qryrel.value(14).toString().trimmed());  // CodNoCli
            }else{
                painter.drawText(05,linha+13,"No cliente:");
                painter.drawText(65,linha+13,qryrel.value(14).toString().trimmed());  // CodNoCli
            }
            painter.setPen(QPen(Qt::black,1));
        }

        int CodNoConcorrente = qryrel.value(21).toString().trimmed().length();
        if(CodNoConcorrente > 0){
            painter.setPen(QPen(Qt::gray,1));
            if(e_PDF_si != "1"){
                painter.drawText(230,linha+131,"No concorrente:");
                painter.drawText(325,linha+131,qryrel.value(21).toString().trimmed());  // CodNoConcorrente
            }else{
                painter.drawText(230,linha+13,"No concorrente:");
                painter.drawText(325,linha+13,qryrel.value(21).toString().trimmed());  // CodNoConcorrente
            }
            painter.setPen(QPen(Qt::black,1));
        }


        QString CodNoFornecedor = qryrel.value(9).toString().trimmed();
        if(CodNoFornecedor != qryrel.value(1).toString().trimmed() && CodNoFornecedor.size() != 0){
            painter.setPen(QPen(Qt::gray,1));
            if(e_PDF_si != "1"){
                painter.drawText(410,linha+131,"No fornecedor:");
                painter.drawText(495,linha+131,qryrel.value(9).toString().trimmed());  // Cod no Fornecedor
            }else{
                painter.drawText(410,linha+13,"No fornecedor:");
                painter.drawText(495,linha+13,qryrel.value(9).toString().trimmed());  // Cod no Fornecedor
            }
            painter.setPen(QPen(Qt::black,1));
        }

        double icms = qryrel.value(22).toDouble();

        if(icms > 0 ){
            if(e_PDF_si != "1"){
                painter.drawText(600,linha+131,"ICMS %:");
                painter.drawText(650,linha+131,qryrel.value(22).toString());  // ICMS %
            }else{
                painter.drawText(600,linha+16,"ICMS %:");
                painter.drawText(650,linha+16,qryrel.value(22).toString());  // ICMS %
            }
        }

        QString Descri =(qryrel.value(3).toString().trimmed());
        int tamDescri = Descri.length();

        if(tamDescri < 40){
            painter.drawText(155,linha, Descri);
        }else{
            if(e_PDF_si != "1"){
                QRectF rectangle2(200, linha+20, 550.0, 95.0);
                painter.setPen(QPen(QColor("white")));
                painter.drawRect(rectangle2) ;
                painter.setPen(QPen(QColor("black")));
                painter.drawText(rectangle2, Qt::TextWordWrap ,Descri) ;
            }else{
                //painter.drawText(155,linha, Descri.left(43));
                QRectF rectangle2(155, linha-18, 285, 46.0);
                painter.setPen(QPen(QColor("white")));
                painter.drawRect(rectangle2) ;
                painter.setPen(QPen(QColor("black")));
                painter.drawText(rectangle2, Qt::TextWordWrap ,Descri) ;
            }
        }

        QString NCM = (qryrel.value(12).toString());
        painter.drawText(445,linha,NCM);                        //NCM

        QString importado = (qryrel.value(17).toString());
        painter.drawText(510,linha,importado);                //importado

        QRectF rectangle3(524, linha-15, 50.0, 18.0); //544
        painter.setPen(QPen(QColor("white")));
        painter.drawRect(rectangle3);
        painter.setPen(QPen(QColor("black")));
        painter.drawText(rectangle3,Qt::AlignCenter, qryrel.value(4).toString()); // Qtde

        QRectF rectangle4(585, linha-13, 52.0, 18.0); //590
        painter.setPen(QPen(QColor("white")));
        painter.drawRect(rectangle4);
        painter.setPen(QPen(QColor("black")));
        double precoUnit = (qryrel.value(5).toDouble());
        painter.drawText(rectangle4,Qt::AlignRight, brasilLocale.toString(precoUnit,'f',2)); // preço unitário

        QString ipi = (qryrel.value(6).toString());
        painter.drawText(647,linha,ipi);

        QRectF rectangle5(679, linha-13, 64.0, 18.0);
        painter.setPen(QPen(QColor("white")));
        painter.drawRect(rectangle5);
        painter.setPen(QPen(QColor("black")));
        double total = (qryrel.value(4).toDouble() * qryrel.value(5).toDouble());                                    // total por item
        painter.drawText(rectangle5,Qt::AlignRight, brasilLocale.toString(total,'f',2)); // sub total

        QString prazo = (qryrel.value(8).toString());
        QRectF rectangle6(751, linha-13, 20.0, 18.0);
        painter.setPen(QPen(QColor("white")));
        painter.setBrush(Qt::cyan);
        painter.drawRect(rectangle6);
        painter.setPen(QPen(QColor("black")));
        painter.drawText(rectangle6,Qt::AlignCenter,prazo);
        painter.setBrush(Qt::white);

        if(e_PDF_si != "1"){
            linha += salto;
        }else{
            linha +=  24;
        }

        if(linha >= qrpp){
            linha = 60;
            pagina = 1;
            printer.newPage();
        }


        vu_i = qryrel.value(5).toDouble();                                              // calculo do desconto
        v_desc = vu_i * (desconto /100);
        s_desc += v_desc ;
        vu = vu_i - v_desc;

        soma_unitario = qryrel.value(4).toDouble() * vu ; // qtde * vu                  // calculo do somatório do cf

        custoFunit = custoF/100 * soma_unitario;
        custoF2 += custoFunit;

        v_frete = (total/totalsub) * val_frete ;                                        // calculo da % do frete para cada item

        valor_ipi = (soma_unitario + custoFunit + v_frete) * (ipi.toDouble()/100);      // calculo do somatório do  ipi
        s_ipi += valor_ipi;

        w_st = qryrel.value(15).toDouble();
        v_st = (soma_unitario + custoFunit + v_frete + valor_ipi) * (w_st/100);         // calculo do somatório do  st
        s_st += v_st;

        //doc.drawContents(&painter);
        // qDebug() << custoF << soma_unitario << custoF2 << s_st << val_frete << valor_ipi << s_ipi<< v_st << s_st;
    }while (qryrel.next());

    if(e_PDF_si != "1"){
        if(linha >= 740){
            printer.newPage();
            linha = 60;
        }
    }

    // linha += 5;


    if(e_PDF_si != "1"){
        linha+=17;
    }else{
        linha+=23;
    }

    painter.setPen(QPen(QColor("blue")));
    painter.drawLine(01,linha-23,780,linha-23);

    painter.setFont(QFont("Tahoma", 14));
    painter.setPen(QPen(Qt::black,5));
    painter.drawText(1,linha,"Condições comerciais:");
    painter.setFont(QFont("Tahoma", 10));
    painter.setPen(QPen(Qt::black,1));
    painter.drawText(1,linha+27,"Valor mínimo para pagamento à vista: ");

    QSqlQuery qrycc(infokapDB);
    qrycc.prepare("Select * from ValorMinimo");
    qrycc.exec();
    qrycc.first();
    double valorAV = qrycc.value(1).toDouble();
    double valor28 = qrycc.value(2).toDouble();
    painter.drawText(225,linha+27,"R$"+brasilLocale.toString(valorAV,'f',2));

    painter.drawText(1,linha+40,"Condições de pagamento: ");
    QString DDL = qrycab.value("DDL").toString();
    if(qrycab.value("DDL").toString() == "à vista"){
        painter.drawText(225,linha+40,"à vista");
    }else{
        QString DDL2 = DDL + " DDL";
        painter.setPen(QPen(Qt::red,1));
        painter.drawText(225,linha+40,DDL2);
        painter.drawText(225,linha+55,"Após aprovação do cadastro.");
        painter.setPen(QPen(Qt::black,1));
    }

    painter.drawText(1,linha + 70,"Valor mínimo para faturamento: ");
    painter.drawText(225,linha + 70,"R$"+brasilLocale.toString(valor28,'f',2));
    painter.drawText(1,linha + 85,"Unidade: peça     Tipo de frete: ");
    QString wfrete = qrycab.value("Frete").toString();
    if(wfrete == "C"){
        painter.drawText(225,linha + 85,"CIF");  //C
    }else{
        painter.drawText(225,linha + 85,"FOB");  //F
    }
    painter.drawText(1,linha + 100,"Solicito informar a transportadora na confirmação do");
    painter.drawText(1,linha + 115,"Pedido de Compra.");
    painter.drawText(1,linha + 130,"Em caso de SEDEX, será cobrada uma taxa conforme");
    painter.drawText(1,linha + 145,"tabela dos Correios.");
    painter.drawText(1,linha + 160,"Garantia: 24 meses a contar da data de entrega contra");
    painter.drawText(1,linha + 175,"defeitos de fabricação.");
    painter.setPen(QPen(QColor("red")));

    painter.drawText(1,linha + 205,"* O Prazo de faturamento é em função do estoque no momento da elaboração do orçamento e pode mudar sem prévio AVISO:");
    painter.drawText(1,linha + 220,"Material destinado a: ( ) Consumo próprio ou ativo imobilizado    ( ) Revenda e Solidário    ( ) Industrialização");
    painter.drawText(1,linha + 235,"Validade monetária deste orçamento: "+QDate::currentDate().addDays(30).toString("dd/MM/yyyy"));
    painter.drawText(1,linha + 250,"IMP. = Importado");
    painter.setPen(QPen(QColor("blue")));
    painter.drawText(310,linha + 235,"Veja os videos dos produtos em: https://www.kap.com.br/biblioteca/videos");

    painter.setPen(QPen(QColor("black")));

    if(qrycab.value("Desconto").toString() != "0"){
        painter.setPen(QPen(Qt::red,1));
        painter.drawText(1,linha + 260,"Prezado Cliente, favor enviar ficha cadastral, contrato social e cartão CNPJ para que possamos cadastrá-lo em nosso sistema.");
        painter.setPen(QPen(Qt::black,1));
    }


    //![01]
    painter.drawRect(QRect(400,linha + 5,240,20));
    painter.drawText(542,linha + 21,"Sub Total (R$):");
    QRectF rectanglesubtot(645, linha + 5, 120.0, 20.0);
    //painter.setPen(QPen(QColor("white")));
    painter.drawRect(rectanglesubtot);
    painter.setPen(QPen(QColor("black")));
    painter.drawText(rectanglesubtot,Qt::AlignRight, brasilLocale.toString(totalsub,'f',2)+"  ");
    //![01]


    //![02]
    painter.drawRect(QRect(400,linha + 27,150,20));
    painter.drawText(403,linha + 42,"Custo financeiro (R$):");
    painter.drawRect(QRect(555,linha + 27,85,20));
    painter.drawText(590,linha + 42,brasilLocale.toString(custoF,'f',2)+"%");
    QRectF rectanglecf(645, linha + 27, 120.0, 20.0);
    //painter.setPen(QPen(QColor("white")));
    painter.drawRect(rectanglecf);
    painter.setPen(QPen(QColor("black")));
    painter.drawText(rectanglecf,Qt::AlignRight, brasilLocale.toString(custoF2,'f',2)+"  ");
    //![02]


    //![03]
    painter.drawRect(QRect(400,linha + 50,150,20));
    painter.drawRect(QRect(555,linha + 50,85,20));
    double desconto2 = 0;
    if(desconto != 0.00){
        desconto2 = desconto / 100 * totalsub;
        painter.drawText(403,linha + 64,"Desconto (R$):");
        painter.drawText(590,linha + 64,brasilLocale.toString(desconto,'f',2)+"%");
    }
    QRectF rectangledesc(645, linha + 50, 120.0, 20.0);
    //painter.setPen(QPen(QColor("white")));
    painter.drawRect(rectangledesc);
    painter.setPen(QPen(QColor("black")));
    if(desconto != 0.00){
        painter.drawText(rectangledesc,Qt::AlignRight, brasilLocale.toString(desconto2,'f',2)+"  ");
    }else{
        painter.drawText(rectangledesc,Qt::AlignRight, " ");
    }
    //![03]


    //![04]
    painter.drawRect(QRect(400,linha + 72,240,20));
    painter.drawText(565,linha + 85,"Frete (R$):");
    QRectF rectanglefrete(645, linha + 72, 120.0, 20.0);
    //painter.setPen(QPen(QColor("white")));
    painter.drawRect(rectanglefrete);
    painter.setPen(QPen(QColor("black")));
    painter.drawText(rectanglefrete,Qt::AlignRight, brasilLocale.toString(val_frete,'f',2)+"  ");
    //![04]


    //![05]
    painter.drawRect(QRect(400,linha + 94,240,20));
    painter.drawText(580,linha + 107,"IPI (R$):");
    QRectF rectangleipi(645, linha + 94, 120.0, 20.0);
    //painter.setPen(QPen(QColor("white")));
    painter.drawRect(rectangleipi);
    painter.setPen(QPen(QColor("black")));
    painter.drawText(rectangleipi,Qt::AlignRight, brasilLocale.toString(s_ipi,'f',2)+"  ");
    //![05]


    //![06]
    painter.drawRect(QRect(400,linha + 116,240,20));
    painter.drawText(583,linha + 129,"ST(R$):");
    QRectF rectanglest(645, linha + 116, 120.0, 20.0);
    //painter.setPen(QPen(QColor("white")));
    painter.drawRect(rectanglest);
    painter.setPen(QPen(QColor("black")));
    painter.drawText(rectanglest,Qt::AlignRight, brasilLocale.toString(s_st,'f',2)+"  ");
    //![06]


    //![07]
    QFont fonte2("Tahoma",10,QFont::Bold,true);
    painter.setFont(fonte2);
    painter.drawRect(QRect(400,linha + 139,240,20));
    painter.drawText(520,linha + 154,"Total Geral (R$):");

    QFont fonte3("Tahoma",10,QFont::Bold,false);
    painter.setFont(fonte3);
    QString OBS = qrycab.value("OBS").toString().trimmed();

    double totalG = totalsub + custoF2 - desconto2 + val_frete + s_ipi + s_st;
    QRectF rectangtot(645, linha + 139, 120.0, 20.0);
    //painter.setPen(QPen(QColor("white")));
    painter.drawRect(rectangtot);
    painter.setPen(QPen(QColor("black")));
    painter.drawText(rectangtot,Qt::AlignRight, brasilLocale.toString(totalG,'f',2)+"  ");
    //![07]

    //qDebug() <<  totalG << totalsub << custoF2 << desconto2 << val_frete << s_ipi << s_st;

    if(qrycab.value("EnviarFichaCadastral").toString() != "0"){           // ficha de bancos
        QSqlQuery qrybancos(infokapDB);
        qrybancos.prepare("select * from Empresa");
        qrybancos.exec();
        qrybancos.first();
        QString banco1 = qrybancos.value(19).toString().trimmed();
        QString banco2 = qrybancos.value(20).toString().trimmed();
        QString banco3 = qrybancos.value(21).toString().trimmed();
        painter.setPen(QPen(Qt::blue,1));
        painter.drawText(25,linha + 280,"Relação de bancos para depósitos em conta corrente.");
        painter.drawText(25,linha + 300,banco1);
        painter.drawText(25,linha + 320,banco2);
        painter.drawText(25,linha + 340,banco3);
        painter.setPen(QPen(Qt::black,1));
    }


    if(qrycab.value("EnviarFichaCadastral").toString() == "0"){           // se não tiver opção de aparecer a ficha de bancos
        linha += 270;
        QRectF rectangle4(0, linha, 760.0, 60.0);
        //painter.setPen(QPen(QColor("white")));
        //painter.setPen(Qt::NoPen);
        painter.drawRect(rectangle4);
        painter.setPen(QPen(QColor("black")));
        if(!OBS.isEmpty()){
            painter.drawText(rectangle4,Qt::TextWordWrap,"Obs: " + OBS);
        }
    }else{
        linha += 370;
        QRectF rectangle5(0, linha, 760.0, 60.0);
        //painter.setPen(QPen(QColor("white")));
        //painter.setPen(Qt::NoPen);
        painter.drawRect(rectangle5);
        painter.setPen(QPen(QColor("black")));
        if(!OBS.isEmpty()){
            painter.drawText(rectangle5,Qt::TextWordWrap, "Obs: " + OBS);
        }
    }

    painter.end();
    if(e_PDF_si == "1"){
        QDesktopServices::openUrl(QUrl("C:/Cotacoes/"+nome));
    }

    //------------------- Abrindo o Outlook para enviar e-mail com o anexo.



    QString Hora;
    QTime horIni;
    if(e_PDF_si != "1"){
        int horario = horIni.currentTime().hour();
        if (horario < 12){
            Hora = "Bom dia, ";
        }else if(horario >= 12 && horario < 18){
            Hora = "Boa tarde, ";
        }else{
            Hora = "Boa noite, ";
        }

        #ifdef _WIN32
        if(e_outlook=="CLAS"){

            QString numOrcam = ui->tw_orcamentos->item(linhaO,0)->text();
            QString at = qrycab.value("Contato").toString();
            QString emailcli = qrycab.value("Email").toString().trimmed().toLower();
            QString assunto_email = "Kap_Orcamento_Numero_"+ui->tw_orcamentos->item(linhaO,0)->text()+"_"+data4 ;
            QString texto = "<h4>"+Hora+at+". "+"Como vai?<br><br>"
                                                        "Segue em anexo o seu orçamento: "
                                                        "Kap Orçamento Número "+numOrcam+"_"+cliente+"_"+data4+"<br><br>"
                                                                       "Ao confirmar o pedido, por gentileza, informe se o material será destinado a: <br>"
                                                                       "( ) Consumo próprio ou ativo imobilizado <br>"
                                                                       "( ) Revenda e solidário <br>"
                                                                       "( ) Industrialização <br><br>"
                                                                       "Essa informação é imprescindível para o faturamento.</h4><br><br>";
            ui->txt_msg->setText(texto);
            ui->txt_msg->selectAll();
            ui->txt_msg->copy();

            QAxObject* outlook = new QAxObject("Outlook.Application",0);
            if (!outlook) {
                qDebug() << "Falha ao iniciar o Outlook.";
                return ;
            }

            QAxObject* mailItem = outlook->querySubObject("CreateItem(int)", 0);
            if (!mailItem) {
                qDebug() << "Falha ao criar o item de e-mail.";
                delete outlook;
                return ;
            }

            QString attachmentPath = "C:/Cotacoes/"+nome;

            if (!attachmentPath.isEmpty()) {
                QAxObject* attachments = mailItem->querySubObject("Attachments");
                if (attachments) {
                    attachments->dynamicCall("Add(const QString&)", attachmentPath);
                }
            }

            QString caminhoAssinatura;
            if(e_vendedor=="000001"){
                caminhoAssinatura = QDir::homePath() +"/AppData/Roaming/Microsoft/Signatures/";
            }else{
                caminhoAssinatura = "C:/Orcamento/";
            }

            QDir dirAssinatura(caminhoAssinatura);
            QStringList filtros;
            filtros << "*.htm";
            dirAssinatura.setNameFilters(filtros);
            QStringList arquivosAssinatura = dirAssinatura.entryList(QDir::Files);
            QString nome_assinatura = arquivosAssinatura.first();
            QString assinatura = pegarAssinaturaEmail(nome_assinatura);

            mailItem->setProperty("Subject", assunto_email);
            mailItem->setProperty("HTMLBody", texto + assinatura);

            QAxObject* recipients = mailItem->querySubObject("Recipients");
            recipients->dynamicCall("Add(QString)", emailcli);

            mailItem->dynamicCall("Display()");

            delete mailItem;
            delete outlook;
        }
#endif
        else if(e_outlook=="novo"){
            qDebug()<< "aqui2";
        }
    }
    e_PDF_si = "0";
    return ;


}

QString orcamento_ti::pegarAssinaturaEmail(const QString& nome_assinatura) {
    // Caminho da pasta de assinaturas do Outlook
    QString caminhoAssinatura;
    if(e_vendedor=="000001"){
        caminhoAssinatura = QDir::homePath() +"/AppData/Roaming/Microsoft/Signatures/";
    }else{
        caminhoAssinatura = "C:/Orcamento/";
    }
    //C:\Users\moreschi.KAP\AppData\Roaming\Microsoft\Signatures


    // Caminho completo do arquivo de assinatura em formato HTML
    QString arquivoAssinatura = caminhoAssinatura + nome_assinatura; // ou ".txt" ou ".rtf"
    //qDebug()<< nome_assinatura << UsuarioRede << arquivoAssinatura;
    QFile arquivo(arquivoAssinatura);
    if (!arquivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //if (!arquivo.open(QIODevice::ReadOnly)) {
        qDebug() << "Erro ao abrir o arquivo de assinatura";
        return QString();
    }

    // Ler todo o conteúdo da assinatura
    QTextStream entrada(&arquivo);
    QString conteudoAssinatura = entrada.readAll();

    arquivo.close();
    return conteudoAssinatura;
}

void orcamento_ti::on_actionAtividades_triggered()
{
    SessaoUsuario s;

    s.vendedor = e_vendedor;
    s.uf = e_uf_vendedor;
    atividades *f_atividades = new atividades(s,this);
    f_atividades->exec();
    delete f_atividades;
}


void orcamento_ti::on_actionCriar_Pedido_triggered()
{
    //![10]
    if(e_vendedor!="000001"){
        //return;
    }
    //![10]


    //![20]
    if(ui->tw_orcamentos->currentRow()==-1){
        QMessageBox::information(this, "AVISO","<h3>Você não informou um orçamento para transfomar em pedido de venda</h3>");
        return;
    }
    //![20]


    //![30]
    int linha = ui->tw_orcamentos->currentRow();
    int id_orca = ui->tw_orcamentos->item(linha,0)->text().toInt();
    QString cod_cli = ui->tw_orcamentos->item(linha,1)->text().trimmed();
    QString loja = ui->tw_orcamentos->item(linha,2)->text().trimmed();
    //![30]


    //![40]     vendo antes se tem TES
    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
    QSqlQuery qry_tes(ProtheusDB);
    qry_tes.prepare("SELECT FM_TS FROM SFM010 "
                    "WHERE FM_CLIENTE = :cod_cli "
                    "AND FM_LOJACLI = :loja "
                    "AND D_E_L_E_T_  <> '*'");
    qry_tes.bindValue(":cod_cli",cod_cli);
    qry_tes.bindValue(":loja",loja);

    if(!qry_tes.exec()){
        QMessageBox::information(this,"AVISO","Falha na consulta qry_tes");
        return;
    }

    if (!qry_tes.next()) {
        QMessageBox::information(this,"AVISO","Cliente/Loja não encontrado na tabela de exceção fiscal (SFM).");
        return;
    }

    QString C6_TES = qry_tes.value(0).toString().trimmed();
    if(C6_TES.isEmpty()){
        QMessageBox::information(this,"AVISO","Não é possível criar pedido para cliente sem TES");
        return;
    }
    //![40]


    //![50]
    if(cod_cli=="000001"){
        QMessageBox::information(this,"AVISO","<h3>Não é possível criar pedido para prospect, Se este já virou cliente, altere o orçamento usando o código do cliente</h3>");
        return;
    }
    //![50]


    //![60]     Verificando se este orçamento já virou pedido
    QSqlDatabase InfokapDB = QSqlDatabase::database("dbi");
    QSqlQuery qry_jvp(InfokapDB);
    qry_jvp.prepare("SELECT TP FROM Cotacoes "
                    "WHERE NDaCotacao = :id_orca");
    qry_jvp.bindValue(":id_orca",id_orca);
    if(!qry_jvp.exec()){
        QMessageBox::information(this,"AVISO","Falha na consulta qry_jvp");
        return;
    }

    if (!qry_jvp.next()) {
        QMessageBox::information(this,"AVISO","Cliente/Loja não encontrado ");
        return;
    }

    if(qry_jvp.value(0).toString().trimmed() == "S"){
        QMessageBox::information(this,"AVISO","<h3>Este orçamento já voi transformado em pedido de venda</h3>");
        return;
    }
    //![60]


    //![70]        Verificando se há produtos no orçamento que não estão cadastrados na tabela SB1.
    // 1. Coleta todos os produtos do orçamento local
    QSqlQuery qry_ler_itens(InfokapDB);
    qry_ler_itens.prepare("SELECT CodigoProduto FROM CotacoesProdutos "
                          "WHERE NDaCotacao = :id_orca AND Imprime = 1");
    qry_ler_itens.bindValue(":id_orca", id_orca);

    if(!qry_ler_itens.exec()){
        QMessageBox::critical(this, "Erro", "Falha ao ler itens do orçamento.");
        return;
    }

    QStringList listaProdutosOrcamento;
    while (qry_ler_itens.next()) {
        QString cod = qry_ler_itens.value(0).toString().trimmed();
        if (!cod.isEmpty()) {
            listaProdutosOrcamento << cod;
        }
    }

    // Se não houver produtos, não precisa validar nada
    if (listaProdutosOrcamento.isEmpty()) return;

    // 2. Monta a Query para o Protheus usando a cláusula IN
    QString listaFormatada = "'" + listaProdutosOrcamento.join("','") + "'";

    QSqlQuery qrypro(ProtheusDB);
    QString sqlProtheus = QString("SELECT B1_COD FROM SB1010 "
                                  "WHERE D_E_L_E_T_ = '' "
                                  "AND B1_COD IN (%1)").arg(listaFormatada);

    if (!qrypro.exec(sqlProtheus)) {
        QMessageBox::warning(this, "Falha", "Erro na validação do Protheus: " + qrypro.lastError().text());
        return;
    }

    // 3. Descobre quais produtos da lista NÃO voltaram na consulta
    QStringList produtosEncontrados;
    while (qrypro.next()) {
        produtosEncontrados << qrypro.value(0).toString().trimmed();
    }

    QStringList produtosFaltantes;
    for (const QString &cod : listaProdutosOrcamento) {
        if (!produtosEncontrados.contains(cod)) {
            produtosFaltantes << cod;
        }
    }

    // 4. Se houver faltantes, exibe todos de uma vez
    if (!produtosFaltantes.isEmpty()) {
        QString mensagem = "Os seguintes produtos não estão cadastrados no Protheus (SB1):\n\n"
                           + produtosFaltantes.join("\n");

        QMessageBox::warning(this, "Produtos Não Cadastrados", mensagem);
        return; // Interrompe o processo pois há erro de cadastro
    }
    // Se chegou aqui, todos os produtos são válidos!
    //![70]


    //![80]     Verificando qual a transportadora E BANCO do cliente
    QSqlQuery qry_transportadora(ProtheusDB) ;
    qry_transportadora.prepare("SELECT A.A1_COD, A.A1_LOJA, A.A1_TRANSP, B.A4_NREDUZ, "
                               "A.A1_TIPO, A.A1_COND, A.A1_VEND, A.A1_COMIS, A.A1_EST, A.A1_BCO1 "
                               "FROM SA1010 A "
                               "LEFT JOIN SA4010 B ON A.A1_TRANSP = B.A4_COD AND B.D_E_L_E_T_ = '' "
                               "WHERE A.A1_COD = :cod_cli "
                               "AND A.A1_LOJA = :loja "
                               "AND A.D_E_L_E_T_ <> '*'"
                               "AND B.D_E_L_E_T_ <> '*'");
    qry_transportadora.bindValue(":cod_cli",cod_cli);
    qry_transportadora.bindValue(":loja",loja);
    if(!qry_transportadora.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta qry_transportadora");
        return;
    }
    if (!qry_transportadora.next()) {
        QMessageBox::warning(this, "AVISO", "Cliente não encontrado no cadastro (SA1).");
        return;
    }

    QString cod_transp = qry_transportadora.value(2).toString().trimmed();
    QString fan_transp = qry_transportadora.value(3).toString().trimmed();
    QString A1_TIPO = qry_transportadora.value(4).toString().trimmed();
    //QString C5_CONDPAG = qry_transportadora.value(5).toString().trimmed();
    QString C5_VEND1 = qry_transportadora.value(6).toString().trimmed();
    QString C5_COMIS1 = qry_transportadora.value(7).toString().replace(',','.');
    QString uf = qry_transportadora.value(8).toString().toUpper();
    QString banco  = qry_transportadora.value(9).toString().toUpper();

    QString A1_TRANSP, qual_transp;
    QMessageBox::StandardButton resposta_transp=QMessageBox::question(this,"TRANSPORTADORA","<h3>CONFIRMA A TRANSPORTADORA "+cod_transp+"  "+fan_transp+"?</h3>",QMessageBox::Yes|QMessageBox::No);
    if(resposta_transp == QMessageBox::No){
        qual_transp = QInputDialog::getText(this,"Transportadora","<h3>Informe o código da nova transportadora. Se não souber, digite N</h3>").toUpper();
        if(qual_transp == "N"){
            sem_transportadora();
            QEventLoop loop;
            loop.exec();
            A1_TRANSP = ui->txt_cod_transp->text();
             qDebug() << A1_TRANSP;
        }else{
            A1_TRANSP = qual_transp;
        }
    }else{
        A1_TRANSP = cod_transp;
    }
    //![80]

    //![85]        BUSCA DO GERENTE
    QSqlQuery qry_gerente(ProtheusDB) ;
    qry_gerente.prepare("SELECT A3_GEREN"
                        " FROM SA3010"
                        " WHERE A3_COD = :vend");
    qry_gerente.bindValue(":vend",C5_VEND1);
    if(!qry_gerente.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta qry_gerente");
        return;
    }
    if (!qry_gerente.next()) {
        QMessageBox::warning(this, "AVISO", "Vendedor não encontrado (SA3).");
        return;
    }
    QString gerente = qry_gerente.value(0).toString().trimmed();
    //![85]


    //![90]     Informações do pedido
    QString C5_PEDCLI = QInputDialog::getText(this,"Pedido do cliente","<h3>Qual o pedido do cliente? </h3>").simplified().left(15);
    if(C5_PEDCLI.isNull()){
        QMessageBox::information(this,"Orçamento","<h3>Não é possível criar pedido sem o pedido do cliente</h3>");
        return;
    }
    QString dataClia = QInputDialog::getText(this,"Pedido do cliente","<h3>Qual a DATA desejada do cliente?  formato: ddmmaa </h3>").simplified();
    if(dataClia.size()>6){
        QMessageBox::information(this,"Data","<h3>Digite apenas 6 números (ddmmaa)</h3>");
        return;
    }
    QString dataCli =  "20" + dataClia.right(2) + dataClia.mid(2,2) + dataClia.left(2);
    QString C5_MENNOTA = QInputDialog::getText(this,"Mensagem para NF","<h3>Mensagem para NF </h3>").simplified();
    QString C5_OBSPV = QInputDialog::getText(this,"Obs do pedido","<h3>Observação para o pedido de venda </h3>").simplified();
    //![90]


    //![100]        Custo financeiro
    QSqlQuery qry_cotacoes(InfokapDB);
    qry_cotacoes.prepare("SELECT Frete, CustoFinanceiro, DDL "
                         "FROM Cotacoes "
                         "WHERE NdaCotacao = :id_orca");

    qry_cotacoes.bindValue(":id_orca",id_orca);

    if(!qry_cotacoes.exec()){
        QMessageBox::information(this,"AVISO","Falha na consulta qry_cotacoes");
        return;
    }
    if (!qry_cotacoes.next()) {
        QMessageBox::warning(this, "AVISO", "Frete não encontrado na tabela Cotacoes.");
        return;
    }
    QString C5_TPFRETE = qry_cotacoes.value("Frete").toString().trimmed();
    double Custo_financeiro = qry_cotacoes.value("CustoFinanceiro").toDouble();

    QString DDL = qry_cotacoes.value("DDL").toString().trimmed().toUpper();
    if(DDL.isEmpty()){
        QMessageBox::information(this,"AVISO","<h3>Este orçamento está sem prazo de pagamento.</h3>");
        return;
    }
    //![100]


     //![110]       Procurando o código do prazo de pagamento em função da descrição de pagamento
    QSqlQuery qry_ver_DDL(InfokapDB);
    qry_ver_DDL.prepare("SELECT E4_CODIGO"
                        " FROM CP"
                        " WHERE CP = :DDL");
    qry_ver_DDL.bindValue(":DDL",DDL);
    qry_ver_DDL.exec();
    if(!qry_ver_DDL.next()){
        QMessageBox::warning(this, "AVISO", "Condição de pagamento não encontrada na tabela CP.");
        return;
    }
    QString C5_CONDPAG = qry_ver_DDL.value("E4_CODIGO").toString();
    //![110]


    //![120]    Peso X Qtde = peso total do pedido
    QSqlQuery qry_CotacoesProdutos(InfokapDB);
    qry_CotacoesProdutos.prepare("SELECT PesoL, Qtde "
                                 "FROM CotacoesProdutos "
                                 "WHERE NdaCotacao = :id_orca "
                                 "AND Imprime = 1");

    qry_CotacoesProdutos.bindValue(":id_orca",id_orca);

    if(!qry_CotacoesProdutos.exec()){
        QMessageBox::information(this,"AVISO","Falha na consulta qry_CotacoesProdutos");
        return;
    }

    if(!qry_CotacoesProdutos.next()){
        QMessageBox::warning(this, "AVISO", "qtde e peso não encontrados na tabela CotacoesProdutos.");
        return;
    }
    double C5_PESOL = 0;
    do{
        double wpeso = qry_CotacoesProdutos.value("PesoL").toDouble();
        int wqtde = qry_CotacoesProdutos.value("Qtde").toDouble();
        C5_PESOL +=  (wpeso * wqtde);
    }while (qry_CotacoesProdutos.next());
    //![120]


     //![130]       Data e hora
    QString C5_EMISSAO = QDate::currentDate().toString("yyyyMMdd");
    QString C5_DATA4 = QDate::currentDate().addDays(10).toString("yyyyMMdd");
    QString hora = QTime::currentTime().toString("hh:mm:ss");
     //![130]


     //![140]      Buscando o último pedido
    QSqlQuery qry_ultimoC5(ProtheusDB) ;
    qry_ultimoC5.prepare("SELECT C5_NUM, R_E_C_N_O_ FROM SC5010 ORDER BY C5_NUM ");
    qry_ultimoC5.exec();
    qry_ultimoC5.last();

    int UltimoPedidoC5 = qry_ultimoC5.value(0).toInt() + 1;
     //![140]


    //![150]        Buscando o último UltimoR_E_C_N_O_C
    QSqlQuery qry_ultimoC5rec(ProtheusDB) ;
    qry_ultimoC5rec.prepare("SELECT C5_NUM, R_E_C_N_O_ FROM SC5010 ORDER BY R_E_C_N_O_ ");
    qry_ultimoC5rec.exec();
    qry_ultimoC5rec.last();

    int UltimoR_E_C_N_O_C5 = qry_ultimoC5rec.value(1).toInt() + 1;
    //![150]


    //![160]        GRAVANDO O CABEÇALHO
    QSqlDatabase ProtheusDBa = QSqlDatabase::database("dba");

    //QSqlQuery qry_cabecalho (InfokapDB);               //não apagar - usado para teste
    //QString sql ="INSERT INTO SC5_local ("    //não apagar - usado para teste
    QSqlQuery qry_cabecalho(ProtheusDBa);

    // 1. Definição dos campos (A lista deve bater exatamente com os Placeholders abaixo)
    QString sql = "INSERT INTO SC5010 ("
                  "C5_FILIAL, C5_NUM, C5_TIPO, C5_CLIENTE, C5_LOJACLI, C5_LOJAENT, C5_CLIENT, C5_TRANSP, "                              //1
                  "C5_TIPOCLI, C5_DTCLI, C5_CONDPAG, C5_TABELA, C5_MENNOTA, C5_MENPAD, C5_OBSPV, C5_PEDCLI, "                   //2
                  "C5_ACRSFIN, C5_VOLUME1, C5_ESPECI1, C5_PBRUTO, C5_PESOL, C5_DESC1, C5_DESC2, C5_DESC3, "                     //3
                  "C5_DESC4, C5_TPFRETE, C5_FRETE, C5_SEGURO, C5_NOTA, C5_VEND1, C5_COMIS1, C5_DESCFI, "                          //4
                  "C5_EMISSAO, C5_PARC1, C5_DATA1, C5_PARC2, C5_DATA2, C5_PARC3, C5_DATA3, C5_DESPESA, "                         //5
                  "C5_FRETAUT, C5_REAJUST, C5_MOEDA, C5_PARC4, C5_DATA4, C5_REIMP, C5_REDESP, C5_SERIE, "                          //6
                  "C5_BANCO, C5_COTACAO, C5_KITREP, C5_TIPLIB, C5_DESCONT, C5_PEDEXP, C5_TPCARGA, C5_PDESCAB, "               //7
                  "C5_BLQ, C5_FORNISS, C5_INCISS, C5_LIBEROK, C5_OK, C5_CONTRA, C5_USERLGI, C5_USERLGA, "                           //8
                  "C5_OS, C5_VEND2, C5_COMIS2, C5_VEND3, C5_TXMOEDA, C5_COMIS3, C5_VEND4, C5_COMIS4, "                            //9
                  "C5_VEND5, C5_COMIS5, C5_VOLUME2, C5_VOLUME3, C5_VOLUME4, C5_ESPECI2, C5_ESPECI3, C5_ESPECI4, "           //10
                  "C5_RECISS, C5_RECFAUT, C5_VLR_FRT, C5_MDCONTR, C5_GERAWMS, C5_MDNUMED, C5_MDPLANI, C5_SOLFRE, "    //11
                  "C5_ORCRES, C5_MUNPRES, C5_DESCMUN, C5_VEICULO, C5_NFSUBST, C5_SERSUBS, C5_EMISSOR, C5_TXREF, "       //12
                  "C5_DTTXREF, C5_MOEDTIT, C5_TP_TRAN, C5_INCOTER, C5_DTLANC, C5_FECENT, C5_SOLOPC, C5_SUGENT, "            //13
                  "C5_CODED, C5_NUMPR, C5_PREPEMB, C5_OBRA, C5_ESTPRES, C5_ORIGEM, D_E_L_E_T_, R_E_C_N_O_, "                  //14
                  "R_E_C_D_E_L_, C5_NUMENT, C5_NTEMPEN, C5_TIPOBRA, C5_INDPRES, C5_NATUREZ, C5_ECSEDEX, C5_ECPRESN, "   //15
                  "C5_ECVINCU, C5_CLIINT, C5_CGCINT, C5_IMINT, C5_DTESERV, C5_CLIRET, C5_LOJARET, C5_MSBLQL, "                      //16
                  "C5_TABTRF, C5_TPCOMPL, C5_REMCTR, C5_REMREV, C5_CODEMB, C5_CNO, C5_FILGCT, C5_SLENVT, "                      //17
                  "C5_RET20G, C5_PLACA2, C5_PLACA1, C5_MODANP, C5_CODVGLP, C5_CODMOT, C5_ARTOBRA, C5_SDOC, "                //18
                  "C5_SDOCSUB, C5_VOLTAPS, C5_PEDECOM, C5_RASTR, C5_STATUS, C5_TRCNUM, C5_CODSAF, C5_FRTCFOP, "         //19
                  "C5_UFDEST, C5_CMUNOR, C5_CMUNDE, C5_UFORIG, C5_CLIREM, C5_LOJAREM, C5_CODA1U"                                    //20
                  ") VALUES ("
                  ":c1,:c2,:c3,:c4,:c5,:c6,:c7,:c8,"                                  //1
                  ":c9,:c10,:c11,:c12,:c13,:c14,:c15,:c16,"                      //2
                  ":c17,:c18,:c19,:c20,:c21,:c22,:c23,:c24,"                    //3
                  ":c25,:c26,:c27,:c28,:c29,:c30,:c31,:c32,"                    //4
                  ":c33,:c34,:c35,:c36,:c37,:c38,:c39,:c40,"                    //5
                  ":c41,:c42,:c43,:c44,:c45,:c46,:c47,:c48,"                    //6
                  ":c49,:c50,:c51,:c52,:c53,:c54,:c55,:c56,"                    //7
                  ":c57,:c58,:c59,:c60,:c61,:c62,:c63,:c64,"                    //8
                  ":c65,:c66,:c67,:c68,:c69,:c70,:c71,:c72,"                    //9
                  ":c73,:c74,:c75,:c76,:c77,:c78,:c79,:c80,"                    //10
                  ":c81,:c82,:c83,:c84,:c85,:c86,:c87,:c88,"                    //11
                  ":c89,:c90,:c91,:c92,:c93,:c94,:c95,:c96,"                    //12
                  ":c97,:c98,:c99,:c100,:c101,:c102,:c103,:c104,"           //13
                  ":c105,:c106,:c107,:c108,:c109,:c110,:c111,:c112,"      //14
                  ":c113,:c114,:c115,:c116,:c117,:c118,:c119,:c120,"      //15
                  ":c121,:c122,:c123,:c124,:c125,:c126,:c127,:c128,"      //16
                  ":c129,:c130,:c131,:c132,:c133,:c134,:c135,:c136,"      //17
                  ":c137,:c138,:c139,:c140,:c141,:c142,:c143,:c144,"      //18
                  ":c145,:c146,:c147,:c148,:c149,:c150,:c151,:c152,"      //19
                  ":c153,:c154,:c155,:c156,:c157,:c158,:c159"                //20
                  ")";

    qry_cabecalho.prepare(sql);

    qry_cabecalho.bindValue(":c1", "01"); // FILIAL
    qry_cabecalho.bindValue(":c2", UltimoPedidoC5);
    qry_cabecalho.bindValue(":c3", "N");
    qry_cabecalho.bindValue(":c4", cod_cli);
    qry_cabecalho.bindValue(":c5", loja);
    qry_cabecalho.bindValue(":c6", loja);
    qry_cabecalho.bindValue(":c7", cod_cli);
    qry_cabecalho.bindValue(":c8", A1_TRANSP);

    qry_cabecalho.bindValue(":c9", A1_TIPO);
    qry_cabecalho.bindValue(":c10", dataCli);
    qry_cabecalho.bindValue(":c11", C5_CONDPAG);
    qry_cabecalho.bindValue(":c12", "001");
    qry_cabecalho.bindValue(":c13", C5_MENNOTA);
    qry_cabecalho.bindValue(":c14", " ");
    qry_cabecalho.bindValue(":c15", C5_OBSPV);
    qry_cabecalho.bindValue(":c16", C5_PEDCLI);

    qry_cabecalho.bindValue(":c17", Custo_financeiro);
    qry_cabecalho.bindValue(":c18", 1);
    qry_cabecalho.bindValue(":c19", "C A I X A");
    qry_cabecalho.bindValue(":c20", 0);
    qry_cabecalho.bindValue(":c21", C5_PESOL);
    qry_cabecalho.bindValue(":c22", 0); // DESC1
    qry_cabecalho.bindValue(":c23", 0); // DESC2
    qry_cabecalho.bindValue(":c24", 0); // DESC3

    qry_cabecalho.bindValue(":c25", 0); // DESC4
    qry_cabecalho.bindValue(":c26", C5_TPFRETE);
    qry_cabecalho.bindValue(":c27", 0); // FRETE
    qry_cabecalho.bindValue(":c28", 0); // SEGURO
    qry_cabecalho.bindValue(":c29", " "); // NOTA
    qry_cabecalho.bindValue(":c30", C5_VEND1);
    qry_cabecalho.bindValue(":c31", C5_COMIS1);
    qry_cabecalho.bindValue(":c32", 0); // DESCFI

    qry_cabecalho.bindValue(":c33", C5_EMISSAO);
    qry_cabecalho.bindValue(":c34", 0); // PARC1
    qry_cabecalho.bindValue(":c35", " "); // DATA1
    qry_cabecalho.bindValue(":c36", 0); // PARC2
    qry_cabecalho.bindValue(":c37", " "); // DATA2
    qry_cabecalho.bindValue(":c38", 0); // PARC3
    qry_cabecalho.bindValue(":c39", dataCli); // DATA3
    qry_cabecalho.bindValue(":c40", 0); // DESPESA

    qry_cabecalho.bindValue(":c41", 0); // FRETAUT
    qry_cabecalho.bindValue(":c42", " "); // REAJUST
    qry_cabecalho.bindValue(":c43", "1"); // MOEDA
    qry_cabecalho.bindValue(":c44", 0); // PARC4
    qry_cabecalho.bindValue(":c45", C5_DATA4);
    qry_cabecalho.bindValue(":c46", 0); // REIMP
    qry_cabecalho.bindValue(":c47", " "); // REDESP
    qry_cabecalho.bindValue(":c48", " "); // SERIE

    qry_cabecalho.bindValue(":c49", banco); // BANCO
    qry_cabecalho.bindValue(":c50", " "); // COTACAO   "+C5_DATA4+" ???
    qry_cabecalho.bindValue(":c51", " "); // KITREP
    qry_cabecalho.bindValue(":c52", "1"); // TIPLIB
    qry_cabecalho.bindValue(":c53", 0); // DESCONT
    qry_cabecalho.bindValue(":c54", " "); // PEDEXP
    qry_cabecalho.bindValue(":c55", "2"); // TPCARGA
    qry_cabecalho.bindValue(":c56", 0); // PDESCAB

    qry_cabecalho.bindValue(":c57", " "); // BLQ
    qry_cabecalho.bindValue(":c58", " "); // FORNISS
    qry_cabecalho.bindValue(":c59", " "); // INCISS
    qry_cabecalho.bindValue(":c60", " "); // LIBEROK
    qry_cabecalho.bindValue(":c61", " "); // OK
    qry_cabecalho.bindValue(":c62", " "); // CONTRA
    qry_cabecalho.bindValue(":c63", " "); // USERLGI ??
    qry_cabecalho.bindValue(":c64", " "); // USERLGA ??

    qry_cabecalho.bindValue(":c65", " "); // OS
    qry_cabecalho.bindValue(":c66", " "); // VEND2
    qry_cabecalho.bindValue(":c67", 0); // COMIS2
    qry_cabecalho.bindValue(":c68", gerente); // VEND3
    qry_cabecalho.bindValue(":c69", 1); // TXMOEDA
    qry_cabecalho.bindValue(":c70", 0); // COMIS3
    qry_cabecalho.bindValue(":c71", " "); // VEND4
    qry_cabecalho.bindValue(":c72", 0); // COMIS4

    qry_cabecalho.bindValue(":c73", " "); // VEND5
    qry_cabecalho.bindValue(":c74", 0); // COMIS5
    qry_cabecalho.bindValue(":c75", 0); // VOL2
    qry_cabecalho.bindValue(":c76", 0); // VOL3
    qry_cabecalho.bindValue(":c77", 0); // VOL4
    qry_cabecalho.bindValue(":c78", " "); // ESPECI2
    qry_cabecalho.bindValue(":c79", " "); // ESPECI3
    qry_cabecalho.bindValue(":c80", " "); // ESPECI4

    qry_cabecalho.bindValue(":c81", " "); // RECISS
    qry_cabecalho.bindValue(":c82", " "); // RECFAUT
    qry_cabecalho.bindValue(":c83", 0); // VLR_FRT
    qry_cabecalho.bindValue(":c84", " "); // MDCONTR
    qry_cabecalho.bindValue(":c85", "1"); // GERAWMS
    qry_cabecalho.bindValue(":c86", " "); // MDNUMED
    qry_cabecalho.bindValue(":c87", " "); // MDPLANI
    qry_cabecalho.bindValue(":c88", " "); // SOLFRE

    qry_cabecalho.bindValue(":c89", " "); // ORCRES
    qry_cabecalho.bindValue(":c90", " "); // MUNPRES
    qry_cabecalho.bindValue(":c91", " "); // DESCMUN
    qry_cabecalho.bindValue(":c92", " "); // VEICULO
    qry_cabecalho.bindValue(":c93", " "); // NFSUBST
    qry_cabecalho.bindValue(":c94", " "); // SERSUBS
    qry_cabecalho.bindValue(":c95", e_sigla); // EMISSOR
    qry_cabecalho.bindValue(":c96", 0); // TXREF

    qry_cabecalho.bindValue(":c97", " "); // DTTXREF
    qry_cabecalho.bindValue(":c98", " "); // MOEDTIT
    qry_cabecalho.bindValue(":c99", 0); // TP_TRAN
    qry_cabecalho.bindValue(":c100", " "); // INCOTER
    qry_cabecalho.bindValue(":c101", " "); // DTLANC
    qry_cabecalho.bindValue(":c102", " "); // FECENT
    qry_cabecalho.bindValue(":c103", " "); // SOLOPC
    qry_cabecalho.bindValue(":c104", "1"); // SUGENT

    qry_cabecalho.bindValue(":c105", " "); // CODED
    qry_cabecalho.bindValue(":c106", " "); // NUMPR
    qry_cabecalho.bindValue(":c107", " "); // PREPEMB
    qry_cabecalho.bindValue(":c108", " "); // OBRA
    qry_cabecalho.bindValue(":c109", " "); // ESTPRES
    qry_cabecalho.bindValue(":c110", " "); // ORIGEM
    qry_cabecalho.bindValue(":c111", " "); // D_E_L_E_T_
    qry_cabecalho.bindValue(":c112", UltimoR_E_C_N_O_C5); // R_E_C_N_O_

    qry_cabecalho.bindValue(":c113", 0); // R_E_C_D_E_L_
    qry_cabecalho.bindValue(":c114", " "); // NUMENT
    qry_cabecalho.bindValue(":c115", " "); // NTEMPEN
    qry_cabecalho.bindValue(":c116", " "); // TIPOBRA
    qry_cabecalho.bindValue(":c117", 0); // INDPRES
    qry_cabecalho.bindValue(":c118", " "); // NATUREZ
    qry_cabecalho.bindValue(":c119", " "); // ECSEDEX
    qry_cabecalho.bindValue(":c120", " "); // ECPRESN

    qry_cabecalho.bindValue(":c121", " "); // ECVINCU
    qry_cabecalho.bindValue(":c122", " "); // CLIINT
    qry_cabecalho.bindValue(":c123", " "); // CGCINT
    qry_cabecalho.bindValue(":c124", " "); // IMINT
    qry_cabecalho.bindValue(":c125", " "); // DTESERV
    qry_cabecalho.bindValue(":c126", " "); // CLIRET
    qry_cabecalho.bindValue(":c127", " "); // LOJARET
    qry_cabecalho.bindValue(":c128", "2"); // MSBLQL

    qry_cabecalho.bindValue(":c129", " "); // TABTRF
    qry_cabecalho.bindValue(":c130", "1"); // TPCOMPL
    qry_cabecalho.bindValue(":c131", " "); // REMCTR
    qry_cabecalho.bindValue(":c132", " "); // REMREV
    qry_cabecalho.bindValue(":c133", " "); // CODEMB
    qry_cabecalho.bindValue(":c134", " "); // CNO
    qry_cabecalho.bindValue(":c135", " "); // FILGCT
    qry_cabecalho.bindValue(":c136", "2"); // SLENVT

    qry_cabecalho.bindValue(":c137", "N"); // RET20G
    qry_cabecalho.bindValue(":c138", " "); // PLACA2
    qry_cabecalho.bindValue(":c139", " "); // PLACA1
    qry_cabecalho.bindValue(":c140", " "); // MODANP
    qry_cabecalho.bindValue(":c141", " "); // CODVGLP
    qry_cabecalho.bindValue(":c142", " "); // CODMOT
    qry_cabecalho.bindValue(":c143", " "); // ARTOBRA
    qry_cabecalho.bindValue(":c144", " "); // SDOC

    qry_cabecalho.bindValue(":c145", " "); // SDOCSUB
    qry_cabecalho.bindValue(":c146", " "); // VOLTAPS
    qry_cabecalho.bindValue(":c147", " "); // PEDECOM
    qry_cabecalho.bindValue(":c148", id_orca); // RASTR
    qry_cabecalho.bindValue(":c149", " "); // STATUS
    qry_cabecalho.bindValue(":c150", " "); // TRCNUM
    qry_cabecalho.bindValue(":c151", " "); // CODSAF
    qry_cabecalho.bindValue(":c152", " "); // FRTCFOP

    qry_cabecalho.bindValue(":c153", " "); // UFDEST
    qry_cabecalho.bindValue(":c154", " "); // CMUNOR
    qry_cabecalho.bindValue(":c155", " "); // CMUNDE
    qry_cabecalho.bindValue(":c156", " "); // UFORIG
    qry_cabecalho.bindValue(":c157", " "); // CLIREM
    qry_cabecalho.bindValue(":c158", " "); // LOJAREM
    qry_cabecalho.bindValue(":c159", " "); // CODA1U

    // 3. Execução
    if(!qry_cabecalho.exec()){
        QMessageBox::warning(this,"FALHA","Falha na gravação do cabeçalho\n"+qry_cabecalho.lastError().text());
        return;
    }else{
        //QMessageBox::warning(this,"OK"," gravação do cabeçalho ok");
    }
    //![160]


    //![170]        GRAVANDO OS ITENS

        // Adicionando itens do orçamento ------------
        // QSqlDatabase ProtheusDB = QSqlDatabase::database("protheus"); // Ajuste para sua conexão de leitura
        // QSqlDatabase ProtheusDBa = QSqlDatabase::database("dba");
        // QSqlDatabase InfokapDB = QSqlDatabase::database("dbi");

        // 1. Busca o último R_E_C_N_O_ da SC6
        QSqlQuery qry_ultimoC6(ProtheusDB);
        qry_ultimoC6.prepare("SELECT MAX(R_E_C_N_O_) FROM SC6010");
        if (!qry_ultimoC6.exec() || !qry_ultimoC6.next()) {
            QMessageBox::critical(this, "Erro", "Não foi possível recuperar o último RECNO da SC6.");
            return;
        }
        int UltimoR_E_C_N_O_C6 = qry_ultimoC6.value(0).toInt();

        // 2. Lê os itens da cotação no Infokap
        QSqlQuery qry_ler_itens2(InfokapDB);
        qry_ler_itens2.prepare("SELECT NDaCotacao, CodigoProduto, nitem, Descrição, Qtde, PreçoUnitario, Total, PerDesc "
                              "FROM CotacoesProdutos "
                              "WHERE NDaCotacao = :id_orca AND Imprime = 1 "
                              "ORDER BY nitem");
        qry_ler_itens2.bindValue(":id_orca", id_orca);

        if (!qry_ler_itens2.exec()) {
            QMessageBox::information(this, "AVISO", "Falha na consulta qry_ler_itens2");
            return;
        }

        int item = 0;
        QLocale brasilLocale(QLocale::Portuguese, QLocale::Brazil);

        while (qry_ler_itens2.next()) {
            item++;
            UltimoR_E_C_N_O_C6++;

            QString nitem = QString::number(item).rightJustified(2, '0');
            QString C6_PRODUTO = qry_ler_itens2.value(1).toString().trimmed().left(15);
            double C6_QTDVEN_VAL = qry_ler_itens2.value(4).toDouble();
            double C6_PRUNIT_VAL = qry_ler_itens2.value(5).toDouble();
            double C6_VALOR_VAL = C6_PRUNIT_VAL * C6_QTDVEN_VAL;

            // 3. Busca Origem e dados contábeis no SB1
            QSqlQuery qry_origem(ProtheusDB);
            qry_origem.prepare("SELECT B1_ORIGEM, B1_CC, B1_CONTA, B1_DESC "
                                         "FROM SB1010 "
                                         "WHERE B1_COD = :cod AND D_E_L_E_T_  <> '*'");
            qry_origem.bindValue(":cod", C6_PRODUTO);
            qry_origem.exec();
            qry_origem.first();

            QString origem = qry_origem.value(0).toString().trimmed();
            QString B1_CC = qry_origem.value(1).toString().trimmed();
            QString B1_CONTA = qry_origem.value(2).toString().trimmed();
            QString C6_DESCRI = qry_origem.value(3).toString().trimmed().left(30).toUpper();

            // 4. Busca FCI (CFD)
            QSqlQuery qry_cfd(ProtheusDB);
            qry_cfd.prepare("SELECT CFD_FCICOD "
                                    "FROM CFD010 "
                                    "WHERE CFD_COD = :cod AND CFD_ORIGEM = :origem AND D_E_L_E_T_  <> '*'");
            qry_cfd.bindValue(":cod", C6_PRODUTO);
            qry_cfd.bindValue(":origem", origem);
            qry_cfd.exec();
            qry_cfd.first();
            QString C6_FCICOD = qry_cfd.value(0).toString().trimmed().left(36);

            // 5. Busca TES (SFM)
            QSqlQuery qry_tes(ProtheusDB);
            qry_tes.prepare("SELECT FM_TS FROM SFM010 WHERE FM_CLIENTE = :cli AND FM_LOJACLI = :loja AND D_E_L_E_T_  <> '*'");
            qry_tes.bindValue(":cli", cod_cli);
            qry_tes.bindValue(":loja", loja);
            qry_tes.exec();
            qry_tes.first();
            QString C6_TES = qry_tes.value(0).toString().trimmed();

            // 6. Situação Tributária e CFOP (SF4)
            QSqlQuery qry_sittrib(ProtheusDB);
            qry_sittrib.prepare("SELECT F4_SITTRIB, F4_CF FROM SF4010 WHERE F4_CODIGO = :tes AND D_E_L_E_T_  <> '*'");
            qry_sittrib.bindValue(":tes", C6_TES);
            qry_sittrib.exec();
            qry_sittrib.first();

            QString F4_SITTRIB = qry_sittrib.value(0).toString().trimmed();
            QString CF = qry_sittrib.value(1).toString().trimmed();

            if (uf != "SP" && uf != "EX") {
                CF = "6" + CF.right(3);
            } else if (uf != "SP" && uf == "EX") {
                CF = "7" + CF.right(3);
            }
            QString C6_CLASFIS = (origem + F4_SITTRIB).left(3);

            // 7. INSERT SC6010

                   // QSqlQuery qry_itens (InfokapDB);                   //não apagar - usado para teste
                   // qry_itens.prepare("INSERT INTO SC6_local ("        //não apagar - usado para teste

            QSqlQuery qry_itens(ProtheusDBa);
            qry_itens.prepare("INSERT INTO SC6010 ("
                              "C6_FILIAL, C6_ITEM, C6_PRODUTO, C6_DESCRI, C6_UM, C6_QTDVEN, C6_PRUNIT, "
                              "C6_PRCVEN, C6_VALOR, C6_TES, C6_CF, C6_PEDCLI, C6_NUMPCOM, C6_ITEMPC, C6_ENTREG, C6_LOCAL, "
                              "C6_CLI, C6_LOJA, C6_NUM, C6_CLASFIS,  C6_SUGENTR, C6_FCICOD, C6_CC, C6_CONTA, "
                              "C6_TPPROD, C6_INTROT, R_E_C_N_O_, C6_MSUIDT, D_E_L_E_T_, C6_OPER, C6_RATEIO) "
                              "VALUES (:filial, :item, :prod, :desc, 'UN', :qtd, :prunit, :prcven, :valor, :tes, :cf, "
                              ":pedcli, :numpcom, :itempc, :entreg, '01', :cli, :loja, :num, :clasfis, :sugent, "
                              ":fci, :cc, :conta, '1', '1', :recno, :uuid, ' ', '01', '2')");

            qry_itens.bindValue(":filial", "01");
            qry_itens.bindValue(":item", nitem);
            qry_itens.bindValue(":prod", C6_PRODUTO);
            qry_itens.bindValue(":desc", C6_DESCRI);
            qry_itens.bindValue(":qtd", C6_QTDVEN_VAL);
            qry_itens.bindValue(":prunit", C6_PRUNIT_VAL);
            qry_itens.bindValue(":prcven", C6_PRUNIT_VAL);
            qry_itens.bindValue(":valor", C6_VALOR_VAL);
            qry_itens.bindValue(":tes", C6_TES);
            qry_itens.bindValue(":cf", CF);
            qry_itens.bindValue(":pedcli", C5_PEDCLI);
            qry_itens.bindValue(":numpcom", C5_PEDCLI);
            qry_itens.bindValue(":itempc", nitem);
            qry_itens.bindValue(":entreg", dataCli);
            qry_itens.bindValue(":cli", cod_cli);
            qry_itens.bindValue(":loja", loja);
            qry_itens.bindValue(":num", QString::number(UltimoPedidoC5));
            qry_itens.bindValue(":clasfis", C6_CLASFIS);
            //qry_itens.bindValue(":frete", C5_TPFRETE);
            qry_itens.bindValue(":sugent", C5_DATA4);
            qry_itens.bindValue(":fci", C6_FCICOD);
            qry_itens.bindValue(":cc", B1_CC);
            qry_itens.bindValue(":conta", B1_CONTA);
            qry_itens.bindValue(":recno", UltimoR_E_C_N_O_C6);
            qry_itens.bindValue(":uuid", QUuid::createUuid().toString().mid(1, 36));

            if (!qry_itens.exec()) {


                //NÃO APAGAR ROTINA ABAIXO - USADA PARA TESTE
                // Deletando o orçamento na tabela SC5_local devido a falha no SC6_local

                // QSqlDatabase InfokapDB = QSqlDatabase::database("dbi");
                // QSqlQuery qry_del_C5_local (InfokapDB);
                // qry_del_C5_local.prepare("UPDATE SC5_local SET "
                //            "C5_NOTA = 'XXXXXX', "
                //            "C5_SERIE = 'XXX', "
                //            "D_E_L_E_T_ = '*', "
                //            "R_E_C_D_E_L_ = :recdel "
                //            "WHERE C5_NUM = :num");
                // qry_del_C5_local.bindValue(":recdel", UltimoR_E_C_N_O_C5);
                // qry_del_C5_local.bindValue(":num", UltimoPedidoC5);
                // qry_del_C5_local.exec();
                // return;


                QMessageBox::warning(this, "FALHA", "Falha na gravação dos itens. Cancelando cabeçalho...");
                QSqlQuery qry_del_C5(ProtheusDBa);
                qry_del_C5.prepare("UPDATE SC5010 SET "
                                   "C5_NOTA = 'XXXXXX', "
                                   "C5_SERIE = 'XXX', "
                                   "D_E_L_E_T_ = '*', "
                                   "R_E_C_D_E_L_ = :recdel "
                                   "WHERE C5_NUM = :num");
                qry_del_C5.bindValue(":recdel", UltimoR_E_C_N_O_C5);
                qry_del_C5.bindValue(":num", UltimoPedidoC5);
                qry_del_C5.exec();
                return;
            }

            // 8. Atualizando Saldo em Poder de Terceiros/Estoque (SB2)
            // QSqlQuery qry_sb2(InfokapDB);          //Usado para teste, não apagar
            // qry_sb2.prepare("SELECT B2_QPEDVEN "
            //                 "FROM SB2_Local "
            //                 "WHERE B2_COD = :prod AND B2_LOCAL = '01' AND D_E_L_E_T_  <> '*'");
            // qry_sb2.bindValue(":prod", C6_PRODUTO);


            QSqlQuery qry_sb2(ProtheusDBa);
            qry_sb2.prepare("SELECT B2_QPEDVEN "
                            "FROM SB2010 "
                            "WHERE B2_COD = :prod AND B2_LOCAL = '01' AND D_E_L_E_T_  <> '*'");
            qry_sb2.bindValue(":prod", C6_PRODUTO);

            if (qry_sb2.exec() && qry_sb2.next()) {
                double B2_QPEDVEN = qry_sb2.value(0).toDouble();
                double novototal = B2_QPEDVEN + C6_QTDVEN_VAL;

                QSqlQuery qry_sb2_u(InfokapDB);                      //não apagar - usado para teste
                qry_sb2_u.prepare("UPDATE SB2_Local SET "        //não apagar - usado para teste
                                  "B2_QPEDVEN = :total, B2_DMOV = :data, B2_HMOV = :hora "
                                  "WHERE B2_COD = :prod AND B2_LOCAL = '01' AND D_E_L_E_T_  <> '*'");
                qry_sb2_u.bindValue(":total", novototal);
                qry_sb2_u.bindValue(":data", C5_EMISSAO);
                qry_sb2_u.bindValue(":hora", hora);
                qry_sb2_u.bindValue(":prod", C6_PRODUTO);
                if(!qry_sb2_u.exec()){
                    QMessageBox::warning(this,"FALHA","Falha na gravação qry_sb2_u");
                    return;
                }




                // QSqlQuery qry_sb2_u(ProtheusDBa);
                // qry_sb2_u.prepare("UPDATE SB2010 SET "
                //                              "B2_QPEDVEN = :total, B2_DMOV = :data, B2_HMOV = :hora "
                //                              "WHERE B2_COD = :prod AND B2_LOCAL = '01' AND D_E_L_E_T_  <> '*'");
                // qry_sb2_u.bindValue(":total", novototal);
                // qry_sb2_u.bindValue(":data", C5_EMISSAO);
                // qry_sb2_u.bindValue(":hora", hora);
                // qry_sb2_u.bindValue(":prod", C6_PRODUTO);
                //                 if(!qry_sb2_u.exec()){
               // QMessageBox::warning(this,"FALHA","Falha na gravação qry_sb2_u");
               // return;

            }
        }

        // 9. Atualiza Status da Cotação no Infokap
        /*
        QSqlQuery qry_tp(InfokapDB);
        qry_tp.prepare("UPDATE Cotacoes SET TP = 'S', VP = 'S' WHERE NdaCotacao = :id");
        qry_tp.bindValue(":id", id_orca);

        if (!qry_tp.exec()) {
            QMessageBox::warning(this, "Cotação", "Falha na atualização do status da cotação.");
            return;
        }
        */
        QMessageBox::information(this, "ORÇAMENTO", "Transferência do orçamento " + QString::number(id_orca) + " concluída com sucesso.");


    //![170]

}




    void orcamento_ti::sem_transportadora()
    {

        if (tv_transportadoras->model()) {
            delete tv_transportadoras->model();
        }

        QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
        QSqlQuery transp(ProtheusDB);

        // 1. SQL Corrigido (Usando <> '*' como combinamos)
        transp.prepare("SELECT A4_COD, A4_NOME, A4_NREDUZ, A4_END, A4_MUN, A4_CGC "
                       "FROM SA4010 "
                       "WHERE D_E_L_E_T_ <> '*' "
                       "ORDER BY A4_NREDUZ");

        if (!transp.exec()) {
            QMessageBox::warning(this, "Erro", "Falha ao consultar transportadoras.");
            return;
        }

        // 2. Lógica de Visibilidade (Toggle)
        if (tv_transportadoras->isVisible()) {
            tv_transportadoras->setVisible(false);
            return;
        }

        // 3. Criando o Model
        QStandardItemModel *model = new QStandardItemModel(this);
        model->setHorizontalHeaderLabels({"Código", "Nome", "Fantasia", "Endereço", "Município", "CNPJ"});

        int rows = 0;
        while(transp.next()){
            model->setItem(rows, 0, new QStandardItem(transp.value("A4_COD").toString().trimmed()));
            model->setItem(rows, 1, new QStandardItem(QString::fromLocal8Bit(transp.value("A4_NOME").toByteArray()).trimmed()));
            model->setItem(rows, 2, new QStandardItem(QString::fromLocal8Bit(transp.value("A4_NREDUZ").toByteArray()).trimmed()));
            model->setItem(rows, 3, new QStandardItem(QString::fromLocal8Bit(transp.value("A4_END").toByteArray()).trimmed()));
            model->setItem(rows, 4, new QStandardItem(QString::fromLocal8Bit(transp.value("A4_MUN").toByteArray()).trimmed()));
            model->setItem(rows, 5, new QStandardItem(transp.value("A4_CGC").toString().trimmed()));
            rows++;
        }

        if(rows == 0){
            QMessageBox::information(this, "TRANSPORTADORA", "Não há transportadoras registradas.");
            delete model;
            return;
        }

        // 4. Configurar a View
        tv_transportadoras->setModel(model);
        connect(tv_transportadoras, &QTableView::doubleClicked, this, &orcamento_ti::selecionarTransportadora);

        // Centralização e Aparência
        int largura = 850;
        int altura = 400;
        // Centraliza em relação à janela principal (this)
        int x = (this->width() - largura) / 2;
        int y = (this->height() - altura) / 2;

        tv_transportadoras->setGeometry(x, y, largura, altura);
        tv_transportadoras->setSelectionBehavior(QAbstractItemView::SelectRows);
        tv_transportadoras->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tv_transportadoras->setSelectionMode(QAbstractItemView::SingleSelection);

        // Ajuste fino das colunas
        tv_transportadoras->setColumnWidth(0, 80);  // Código
        tv_transportadoras->setColumnWidth(1, 200); // Nome
        tv_transportadoras->setColumnWidth(2, 150); // Fantasia
        tv_transportadoras->setColumnWidth(3, 200); // Endereço

        tv_transportadoras->show();
        tv_transportadoras->raise();

}



void orcamento_ti::selecionarTransportadora(const QModelIndex &index)
{
    ui->txt_cod_transp->setText(index.sibling(index.row(), 0).data().toString());
    tv_transportadoras->setVisible(false);
    disconnect(tv_transportadoras, &QTableView::doubleClicked, this, &orcamento_ti::selecionarTransportadora);
}


bool orcamento_ti::eventFilter(QObject *obj, QEvent *event) {
    if (obj == tv_transportadoras && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Escape) {
            tv_transportadoras->setVisible(false);
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}


void orcamento_ti::on_bt_visualizar_clicked()
{
    e_PDF_si = "1";
    on_bt_visualizarPDF_clicked();
}


void orcamento_ti::on_bt_ver_todos_clicked()
{
    e_todas = QInputDialog::getText(this,"FILTRO","Quantas linhas deseja ver?").toUpper();

    if(e_todas.isEmpty()){
        return;
    }

    atualizarTi();

}


void orcamento_ti::on_bt_prospects_clicked()
{
    SessaoUsuario atual;

    atual.vendedor = e_vendedor;
    atual.uf       = e_uf_vendedor;

    prospect_ti  *tela  = new prospect_ti(atual, this);
    tela->exec();
}


void orcamento_ti::on_bt_catalogo_clicked()
{
    QDesktopServices::openUrl(QUrl("S:/br/"));
}


void orcamento_ti::on_actionPedidos_triggered()
{
    SessaoUsuario atual;

    atual.vendedor = e_vendedor;
    atual.regiao     = e_regiao;

    pedidos *tela  = new pedidos(atual, this);
    tela->exec();
}


void orcamento_ti::on_actionAtendente_triggered()
{
    e_sigla_filtro = QInputDialog::getText(this,"FILTRO","Qual a sigla do atendente?");
    if(e_sigla_filtro.isNull()){
        return;
    }
    atualizarTi();
}


void orcamento_ti::on_actionC_digo_do_cliente_triggered()
{
    e_codcli_filtro = QInputDialog::getText(this,"FILTRO","Qual o código do cliente?");
    if(e_codcli_filtro.isNull()){
        return;
    }
    atualizarTi();
}


void orcamento_ti::on_actionFantasia_triggered()
{
    e_nomecli_filtro = QInputDialog::getText(this,"FILTRO","Qual o nome fantasia do cliente?");
    if(e_nomecli_filtro.isNull()){
        return;
    }
    atualizarTi();
}


void orcamento_ti::on_actionEstado_triggered()
{
    e_uf_filtro = QInputDialog::getText(this,"FILTRO","Qual a sigla do estado?").toUpper();
    if(e_uf_filtro.isNull()){
        return;
    }
    atualizarTi();
}


void orcamento_ti::on_actionProspect_triggered()
{
    e_prospect_filtro = QInputDialog::getText(this,"FILTRO","Qual o nome fantasia do prospect?");
    if(e_prospect_filtro.isNull()){
        return;
    }
    atualizarTi();
}


void orcamento_ti::on_actionE_mail_do_cliente_triggered()
{
    e_Email_filtro = QInputDialog::getText(this,"FILTRO","Qual o e-mail do cliente?").toLower();
    if(e_Email_filtro.isNull()){
        return;
    }
    atualizarTi();
}


void orcamento_ti::on_actionRegi_o_triggered()
{
    e_regiao_filtro = QInputDialog::getText(this,"FILTRO","Qual a região?");
    if(e_regiao_filtro.isNull()){
        return;
    }
    atualizarTi();
}


void orcamento_ti::on_actionLimpar_filtro_triggered()
{
    e_limpar_filtro = "N";
    e_Email_filtro.clear();
    e_nomecli_filtro.clear();
    e_sigla_filtro.clear();
    e_prospect_filtro.clear();
    e_codcli_filtro.clear();
    e_uf_filtro.clear();
    e_regiao_filtro.clear();
   // Filtro = "1";
    atualizarTi();
}


void orcamento_ti::on_actionN_mero_triggered()
{
    QString codigo = QInputDialog::getText(this,"Orçamento","Qual o número do orçamento?");
    if(codigo.isNull()){
        atualizarTi() ;
        return;
    }

    int colunaOP =0;
    bool encontrado = false;

    for (int linha = 0; linha < ui->tw_orcamentos->rowCount(); ++linha) {
        QTableWidgetItem *item = ui->tw_orcamentos->item(linha, colunaOP);
        if (item) {
            QString texto_tabela = item->text().trimmed().toUpper();
            if (texto_tabela == codigo) {
                encontrado = true;
                ui->tw_orcamentos->selectRow(linha);
                ui->tw_orcamentos->scrollToItem(item);
                ui->tw_orcamentos->setFocus();
                break;
            }
        }
    }

    if (!encontrado) {
        QMessageBox::information(this, "AVISO", "<h3>Orçamento " + codigo + " não encontrado - Clique em Ver + e faça a pesquisa novamente.</h3>");
        return;
    }
}


void orcamento_ti::on_actionDeletar_triggered()
{

        // 1. VALIDAÇÃO DE SELEÇÃO
        int linha = ui->tw_orcamentos->currentRow();
        if(linha == -1){
            QMessageBox::information(this, "AVISO", "<h3>Você não selecionou um orçamento para deletar.</h3>");
            return;
        }

        // 2. CAPTURA DO ID
        // Dica: Pegar o texto da coluna 0 (NDaCotacao)
        QString idStr = ui->tw_orcamentos->item(linha, 0)->text();
        int id = idStr.toInt();

        // 3. CONFIRMAÇÃO DO USUÁRIO
        QMessageBox::StandardButton opc = QMessageBox::question(
            this, "DELETAR",
            "<h3>Confirma marcar como deletado o orçamento</h3> " + idStr + "?",
            QMessageBox::No | QMessageBox::Yes
            );

        if (opc != QMessageBox::Yes) return;

        // 4. EXECUÇÃO NO BANCO DE DADOS
        QSqlDatabase InfokapDB = QSqlDatabase::database("dbi");

        // Iniciar uma transação garante que ou deleta TUDO ou nada
        InfokapDB.transaction();

        QSqlQuery qry(InfokapDB);

        // Atualiza a tabela principal
        qry.prepare("UPDATE Cotacoes SET Deletado = '*' WHERE NDaCotacao = :id");
        qry.bindValue(":id", id);

        bool okPrincipal = qry.exec();

        // Atualiza os itens
        qry.prepare("UPDATE CotacoesProdutos SET Deletado = '*' WHERE NDaCotacao = :id");
        qry.bindValue(":id", id);

        bool okItens = qry.exec();

        if (okPrincipal && okItens) {
            InfokapDB.commit(); // Grava as alterações
            atualizarTi();
        } else {
            InfokapDB.rollback(); // Cancela se der erro em algum
            QMessageBox::critical(this, "Erro", "Falha ao deletar: " + qry.lastError().text());
        }
}


void orcamento_ti::on_actionRela_o_de_Clientes_triggered()
{

    bool ok;
    QString escolha, escolha2, listapor;
    QStringList items, items2;

    items << tr("Completa") << tr("Só os ativos");
    QString item = QInputDialog::getItem(this, tr("Lista"), tr("Exibir:"), items, 0, false, &ok);
    if (!ok || item.isEmpty()) return;
    escolha = item;

    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
    QSqlQuery qry_clientes(ProtheusDB);

    // 1. Definição da Base da Query (Evita repetir o SELECT em todo IF)
    QString sqlBase = "SELECT A1_COD, A1_LOJA, A1_NREDUZ, A1_TIPO, A1_CGC, A1_MUN, A1_EST, "
                      "A1_DDD, A1_TEL, A1_EMLCOM, A1_REGIAO, A1_CONTATO FROM SA1010 WHERE ";
    QString filtro;
    QMap<QString, QVariant> params;

    // 2. Lógica de Filtros por Vendedor
    if (e_vendedor != "000001") {
        if (e_vendedor == "0063") {
            filtro = "A1_EST = :uf";
            params[":uf"] = e_uf_vendedor;
        } else if (e_vendedor == "0089") {
            filtro = "A1_REGIAO = :regiao";
            params[":regiao"] = "RS0";
        }
    } else {
        // Lógica para Admin (000001)
        items2 << "Por Região" << "Por estado" << "Por vendedor";
        escolha2 = QInputDialog::getItem(this, "Opções", "Listar por:", items2, 0, false, &ok);
        if (!ok) return;

        if (escolha2 == "Por Região") {
            listapor = QInputDialog::getText(this, "Região", "Qual região?").toUpper();
            filtro = "A1_REGIAO = :regiao";
            params[":regiao"] = listapor;
        } else if (escolha2 == "Por estado") {
            listapor = QInputDialog::getText(this, "Estado", "UF:").toUpper();
            filtro = "A1_EST LIKE :uf";
            params[":uf"] = "%" + listapor;
        } else if (escolha2 == "Por vendedor") {
            listapor = QInputDialog::getText(this, "Vendedor", "Código:").trimmed().toUpper();
            filtro = "A1_VEND = :vend";
            params[":vend"] = listapor;
        }
    }

    // 3. Adiciona filtro de ativos se necessário
    if (escolha != "Completa") {
        filtro = "A1_MSBLQL = '2' AND (" + filtro + ")";
    }

    // 4. Prepara e faz os Binds de forma dinâmica
    qry_clientes.prepare(sqlBase + filtro + " ORDER BY A1_NREDUZ");

    QMapIterator<QString, QVariant> i(params);
    while (i.hasNext()) {
        i.next();
        qry_clientes.bindValue(i.key(), i.value());
    }

    if (!qry_clientes.exec()) {
        QMessageBox::warning(this, "ERRO", "Falha na consulta.");
        return;
    }

    /*
    bool ok;
    QString escolha, escolha2, listapor;
    QStringList items, items2;

    items << tr("Completa") << tr("Só os ativos");
    QString item = QInputDialog::getItem(this, tr("Lista::getItem()"), tr("Escolha:"), items, 0, false, &ok);
    if (ok && !item.isEmpty())
        escolha = item;

    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
    QSqlQuery qry_clientes(ProtheusDB);

    if (e_vendedor != "000001") {
        if (e_vendedor == "0063") {
            if (escolha == "Completa") {
                qry_clientes.prepare("SELECT A1_COD, A1_LOJA, A1_NREDUZ, A1_TIPO, A1_CGC, A1_MUN, A1_EST, "
                                     "A1_DDD, A1_TEL, A1_EMLCOM FROM SA1010 WHERE A1_EST = :uf ORDER BY A1_NREDUZ");
            } else {
                qry_clientes.prepare("SELECT A1_COD, A1_LOJA, A1_NREDUZ, A1_TIPO, A1_CGC, A1_MUN, A1_EST, "
                                     "A1_DDD, A1_TEL, A1_EMLCOM FROM SA1010 WHERE A1_MSBLQL=2 AND A1_EST = :uf ORDER BY A1_NREDUZ");
            }
            qry_clientes.bindValue(":uf", e_uf_vendedor);
        }

        if (e_vendedor == "0089") {
            QString regiao = "RS0";
            if (escolha == "Completa") {
                qry_clientes.prepare("SELECT A1_COD, A1_LOJA, A1_NREDUZ, A1_TIPO, A1_CGC, A1_MUN, A1_EST, "
                                     "A1_DDD, A1_TEL, A1_EMLCOM, A1_REGIAO, A1_CONTATO FROM SA1010 WHERE A1_REGIAO = :regiao ORDER BY A1_NREDUZ");
            } else {
                qry_clientes.prepare("SELECT A1_COD, A1_LOJA, A1_NREDUZ, A1_TIPO, A1_CGC, A1_MUN, A1_EST, "
                                     "A1_DDD, A1_TEL, A1_EMLCOM, A1_REGIAO, A1_CONTATO FROM SA1010 WHERE A1_MSBLQL=2 AND A1_REGIAO = :regiao ORDER BY A1_NREDUZ");
            }
            qry_clientes.bindValue(":regiao", regiao);
        }

        if (!qry_clientes.exec()) {
            QMessageBox::warning(this, "ERRO", "Falha na consulta de listagem de clientes");
        }
    }

    if (e_vendedor == "000001") {
        items2 << ("Por Região") << ("Por estado") << ("Por vendedor");
        QString item2 = QInputDialog::getItem(this, tr("Lista::getItem()"), tr("Escolha:"), items2, 0, false, &ok);
        if (ok && !item2.isEmpty())
            escolha2 = item2;

        if (escolha2 == "Por Região") {
            QString regiao = QInputDialog::getText(this, "Lista", "Qual a região? (sem acentuações)").toUpper();
            listapor = regiao;
            if (escolha == "Completa") {
                qry_clientes.prepare("SELECT A1_COD, A1_LOJA, A1_NREDUZ, A1_TIPO, A1_CGC, A1_MUN, A1_EST, "
                                     "A1_DDD, A1_TEL, A1_EMLCOM, A1_REGIAO, A1_CONTATO FROM SA1010 WHERE A1_REGIAO = :regiao ORDER BY A1_NREDUZ");
            } else {
                qry_clientes.prepare("SELECT A1_COD, A1_LOJA, A1_NREDUZ, A1_TIPO, A1_CGC, A1_MUN, A1_EST, "
                                     "A1_DDD, A1_TEL, A1_EMLCOM, A1_REGIAO, A1_CONTATO FROM SA1010 WHERE A1_MSBLQL=2 AND A1_REGIAO = :regiao ORDER BY A1_NREDUZ");
            }
            qry_clientes.bindValue(":regiao", regiao);
        }

        if (escolha2 == "Por estado") {
            QString uf = QInputDialog::getText(this, "Lista", "Qual a sigla do estado?").toUpper();
            listapor = uf;
            if (escolha == "Completa") {
                qry_clientes.prepare("SELECT A1_COD, A1_LOJA, A1_NREDUZ, A1_TIPO, A1_CGC, A1_MUN, A1_EST, "
                                     "A1_DDD, A1_TEL, A1_EMLCOM, A1_REGIAO, A1_CONTATO FROM SA1010 WHERE A1_EST LIKE :uf ORDER BY A1_NREDUZ");
            } else {
                qry_clientes.prepare("SELECT A1_COD, A1_LOJA, A1_NREDUZ, A1_TIPO, A1_CGC, A1_MUN, A1_EST, "
                                     "A1_DDD, A1_TEL, A1_EMLCOM, A1_REGIAO, A1_CONTATO FROM SA1010 WHERE A1_MSBLQL=2 AND A1_EST LIKE :uf ORDER BY A1_NREDUZ");
            }
            qry_clientes.bindValue(":uf", "%" + uf);
        }

        if (escolha2 == "Por vendedor") {
            QString vendedor = QInputDialog::getText(this, "Lista", "Qual o código do vendedor?").toUpper().trimmed();
            listapor = vendedor;
            if (escolha == "Completa") {
                qry_clientes.prepare("SELECT A1_COD, A1_LOJA, A1_NREDUZ, A1_TIPO, A1_CGC, A1_MUN, A1_EST, "
                                     "A1_DDD, A1_TEL, A1_EMLCOM, A1_REGIAO, A1_CONTATO FROM SA1010 WHERE A1_VEND = :vend ORDER BY A1_NREDUZ");
            } else {
                qry_clientes.prepare("SELECT A1_COD, A1_LOJA, A1_NREDUZ, A1_TIPO, A1_CGC, A1_MUN, A1_EST, "
                                     "A1_DDD, A1_TEL, A1_EMLCOM, A1_REGIAO, A1_CONTATO FROM SA1010 WHERE A1_MSBLQL=2 AND A1_VEND = :vend ORDER BY A1_NREDUZ");
            }
            qry_clientes.bindValue(":vend", vendedor);
        }

        if (!qry_clientes.exec()) {
            QMessageBox::warning(this, "ERRO", "Falha na consulta de listagem de clientes");
        }
    }
    */

    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    QString nome = "ListaDeClientes.pdf";
    printer.setOutputFileName("C:/Cotacoes/" + nome);
    QPainter painter;
    if (!painter.begin(&printer)) {
        return;
    }

    if (!qry_clientes.first()) return;

    QPixmap logo("U:/Biblioteca/Imagens_Gerais/Logos+Usados/kap_top_cor.gif");
    painter.drawPixmap(30, 05, QPixmap(logo.scaled(200, 300, Qt::KeepAspectRatio)));
    painter.setFont(QFont("Tahoma", 22));
    painter.drawText(300, 25, "Lista de clientes");
    painter.setFont(QFont("Tahoma", 12));
    painter.drawText(330, 45, QDateTime::currentDateTime().toString("dd/MM/yyyy") + " - " + listapor);

    int linha = 100;
    int salto = 20;
    int pagina = 1;
    int qrpp = 1010;
    int qdelinha = 1;

    do {
        qdelinha++;
    } while (qry_clientes.next());
    qry_clientes.first();
    int qpag = (qdelinha / 48) + 1;

    painter.setPen(QPen(Qt::blue, 1));
    painter.setFont(QFont("Tahoma", 8));
    painter.drawText(0, 70, "Código");
    painter.drawText(40, 70, "Loja");
    painter.drawText(65, 70, "Nome fantasia");
    painter.drawText(190, 70, "Tipo");
    painter.drawText(220, 70, "CNPJ");
    painter.drawText(330, 70, "Município");
    painter.drawText(400, 70, "Contato");
    painter.drawText(490, 70, "Telefone");
    painter.drawText(557, 70, "E-mail");
    painter.drawLine(0, 80, 770, 80);
    painter.setPen(QPen(Qt::black, 1));

    do {
        painter.drawText(0, linha, qry_clientes.value(0).toString());
        painter.drawText(40, linha, qry_clientes.value(1).toString());
        painter.drawText(65, linha, qry_clientes.value(2).toString().left(20));
        painter.drawText(200, linha, qry_clientes.value(3).toString());

        QString rawCnpj = qry_clientes.value(4).toString();
        QString cnpj = rawCnpj.left(2) + "." + rawCnpj.mid(2, 3) + "." + rawCnpj.mid(5, 3) + "/" + rawCnpj.mid(8, 4) + "-" + rawCnpj.right(2);
        painter.drawText(220, linha, cnpj);
        painter.drawText(330, linha, qry_clientes.value(5).toString().left(9));

        QString xxs = qry_clientes.value(11).toString().trimmed().toLower();
        QString xxs2 = xxs.isEmpty() ? "" : xxs.left(1).toUpper() + xxs.mid(1);

        painter.drawText(400, linha, xxs2);
        painter.drawText(475, linha, qry_clientes.value(7).toString());
        painter.drawText(500, linha, qry_clientes.value(8).toString());
        painter.drawText(557, linha, qry_clientes.value(9).toString());

        linha += salto;
        if (linha >= qrpp) {
            painter.drawText(700, linha, "Página: " + QString::number(pagina) + " de " + QString::number(qpag));
            linha = 30;
            pagina++;
            printer.newPage();
            painter.setPen(QPen(Qt::blue, 1));
            painter.setFont(QFont("Tahoma", 8));
            painter.drawText(1, 20, "Código");
            painter.drawText(40, 20, "Loja");
            painter.drawText(65, 20, "Nome fantasia");
            painter.drawText(190, 20, "Tipo");
            painter.drawText(220, 20, "CNPJ");
            painter.drawText(330, 20, "Município");
            painter.drawText(400, 20, "Contato");
            painter.drawText(490, 20, "Telefone");
            painter.drawText(557, 20, "E-mail");
            painter.drawLine(0, 30, 770, 30);
            linha += 20;
            painter.setPen(QPen(Qt::black, 1));
        }
    } while (qry_clientes.next());

    painter.drawText(700, linha, "Página: " + QString::number(pagina) + " de " + QString::number(qpag));
    painter.end();
    QDesktopServices::openUrl(QUrl::fromLocalFile("C:/Cotacoes/" + nome));


}


void orcamento_ti::on_bt_editar_clicked()
{

    if(ui->tw_orcamentos->currentRow()==-1){
        QMessageBox::information(this,"AVISO","<h3>Você não informou um orçamento para editar</h3>");
        return;
    }
    int linha =ui->tw_orcamentos->currentRow();
    e_id = ui->tw_orcamentos->item(linha,0)->text().toInt();

    SessaoUsuario atual;

    atual.sigla        = e_sigla;
    atual.vendedor  = e_vendedor;
    atual.id            = e_id;

    orcamento_editar  *telaE  = new orcamento_editar(atual, this);
    telaE->exec();

    atualizarTi();
}

