#include "orcamento_novo.h"
#include "ui_orcamento_novo.h"
//#include "orcamento_ti.h"
#include "editar_item.h"
#include "dados_do_cliente.h"

QString e_qtde, e_codp;
double Desconto = 0;
double Desconto_excel = 0;
double e_livre = 0;
double e_tramite = 0;
int e_linha = 0;                //usado na

QString m_contato_original;
QString m_email_original;
QString m_ddd_original;
QString m_telefone_original;

orcamento_novo::orcamento_novo(SessaoUsuario sessao, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::orcamento_novo)
    , m_dados(sessao) // Copia o pacote recebido para a variável m_dados

{
    ui->setupUi(this);

    this->showFullScreen();

    QSqlDatabase defaultDB = QSqlDatabase::database();
    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");


    // Criando a instância
    tv_ultimasVendas = new QTableView(this);
    tv_ultimasVendas->setStyleSheet("background-color: rgb(255,255,200); selection-background-color: blue;");
    tv_ultimasVendas->setWindowFlags(Qt::SubWindow); // Opcional: para dar um aspecto de painel
    tv_ultimasVendas->setVisible(false);

    tv_oqueComprou = new QTableView(this);
    tv_oqueComprou ->setStyleSheet("background-color: rgb(100,255,255); selection-background-color: blue;");
    tv_oqueComprou->setWindowFlags(Qt::SubWindow);
    tv_oqueComprou->setVisible(false);

    ui->bt_importar_excel->setEnabled(false);

    QSqlDatabase InfokapDB = QSqlDatabase::database("dbi");

    //CARREGANDO O COMBOBOX FUNCIONÁRIOS

    ui->cb_atendente->setCurrentText(sessao.sigla);

    // if((orcamento_ti::e_sigla) == "CRM"){
    //     ui->txt_qtde_2->setVisible(true);
    //     ui->txt_qtde_3->setVisible(true);
    //     ui->txt_qtde_4->setVisible(true);
    //     ui->txt_qtde_5->setVisible(true);
    // }else{
    //     ui->txt_qtde_2->setVisible(false);
    //     ui->txt_qtde_3->setVisible(false);
    //     ui->txt_qtde_4->setVisible(false);
    //     ui->txt_qtde_5->setVisible(false);
    // }

    //CARREGANDO OS COMBOBOX CP_CF ----- INÍCIO
    QLocale brasilLocale(QLocale::Portuguese,QLocale::Brazil);
    QSqlQuery qry_cpcf(InfokapDB);
    qry_cpcf.prepare("SELECT CP FROM CP");
    if(!qry_cpcf.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta qry_cpcf\n"+qry_cpcf.lastError().text());
        return;
    }

    QStandardItemModel *modelcp = new QStandardItemModel (this);
    while (qry_cpcf.next()) {
        modelcp->appendRow(new QStandardItem(qry_cpcf.value(0).toString()));
    }
    ui->cb_dll->setModel(modelcp);
    ui->cb_dll->setCurrentText("");
    //CARREGANDO OS COMBOBOX CP_CF ----- FIM

    ui->txt_nomeCli->setFocus();
    ui->tw_produtos->setColumnCount(20);

    ui->tw_produtos->setColumnWidth(0,100);
    ui->tw_produtos->setColumnWidth(1,120);
    ui->tw_produtos->setColumnWidth(2,210);
    ui->tw_produtos->setColumnWidth(3,70);
    ui->tw_produtos->setColumnWidth(4,60);
    ui->tw_produtos->setColumnWidth(5,50);
    ui->tw_produtos->setColumnWidth(6,60);
    ui->tw_produtos->setColumnWidth(7,40);
    ui->tw_produtos->setColumnWidth(8,80);
    ui->tw_produtos->setColumnWidth(9,70);
    ui->tw_produtos->setColumnWidth(10,45);
    ui->tw_produtos->setColumnWidth(11,80);
    ui->tw_produtos->setColumnWidth(12,1);
    ui->tw_produtos->setColumnWidth(13,1);
    ui->tw_produtos->setColumnWidth(14,20);
    ui->tw_produtos->setColumnWidth(15,1);
    ui->tw_produtos->setColumnWidth(16,120);
    ui->tw_produtos->setColumnWidth(17,50);
    ui->tw_produtos->setColumnWidth(18,120);
    ui->tw_produtos->setColumnWidth(19,15);
    QStringList cabecalhos={"Código Kap","Código no cliente","Descrição", "Desconto\n%","Preço \nunitário","Qtde","Prazo p/\nfaturar",
                              "IPI %","Valor total","N.C.M.","S.T.%", "Preço cheio","I","Ig","Peso","Imprime","Código no\nConcorrente","ICMS %","Código no\nFornecedor","Grupo"};
    ui->tw_produtos->setHorizontalHeaderLabels(cabecalhos);
    if(ui->txt_bloqueado->text() == "S"){
        ui->tw_produtos->setStyleSheet("QTableView {SELECTion-background-color: #FF0000; SELECTion-color: #0000FF;}");
    }else{
        ui->tw_produtos->setStyleSheet("QTableView {SELECTion-background-color: #000000; SELECTion-color: #FFFFFF;}");
    }

    ui->tw_produtos->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tw_produtos->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tw_produtos->verticalHeader()->setVisible(true);



}

orcamento_novo::~orcamento_novo()
{
    delete ui;
}

void orcamento_novo::on_bt_voltar_clicked()
{
    close();
}


void orcamento_novo::on_txt_codP_editingFinished()
{

    QString codp, cod_original;
    codp = ui->txt_codP->text().toUpper().simplified();

    int contLi = ui->tw_produtos->rowCount();
    ui->tw_produtos->insertRow(contLi);

    //![10]  //--------------- RETIRAR ESPAÇOS DENTRO DO CÓDIGO  ---------------
    int tamanho = codp.size();
    QString codp_e, codP_d;
    int dig_direita = 0;
    int dig_esquerda = 0;
    int p = 0;

    for (int i = 0; i < tamanho; i++) {
        p++;
        if(codp.mid(i,1) == " "){
            dig_direita = tamanho - p;
            dig_esquerda = (tamanho - dig_direita) - 1;
            codP_d = codp.right(dig_direita);
            codp_e = codp.left(dig_esquerda);
            codp = codp_e + codP_d;
        }
    }
      ui->tw_produtos->setItem(contLi,0,new QTableWidgetItem(codp)); //código na Kap
    //![10]  //---------------RETIRAR ESPAÇOS DENTRO DO CÓDIGO ---------------//


     //![20] //---------- CASO TENHA DIGITADO O CÓDIGO DO PRODUTO DO CONCORRENTE -------------//
    QString codNoConcorrente = ui->txt_cod_concorrente->text().left(20);
    ui->tw_produtos->setItem(contLi,16,new QTableWidgetItem(codNoConcorrente)); //código no concorrente
    ui->txt_cod_concorrente->clear();
    ui->tw_produtos->setItem(contLi,17,new QTableWidgetItem(codNoConcorrente)); //ICMS %

    QString qtde = ui->txt_qtde->text();
    QTableWidgetItem *qtde_a = new QTableWidgetItem(qtde);
    ui->tw_produtos->setItem(contLi,5,qtde_a);
    qtde_a->setTextAlignment(Qt::AlignCenter);
     //![20]//---------- CASO TENHA DIGITADO O CÓDIGO DO PRODUTO DO CONCORRENTE -------------//


    if(codp.isEmpty()){
        return;
    }


    e_codp = codp;
    cod_original = codp;



     //![30] //---------- LENDO VALORES DA TABELA SB1 ----------------//
    QLocale brasilLocale(QLocale::Portuguese,QLocale::Brazil);
    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
    QSqlQuery qrypro(ProtheusDB);
    qrypro.prepare("SELECT B1_DESC"
                   ",B1_QTDEMB"
                   ",B1_PESBRU"
                   ",B1_IMPORT"
                   ",B1_IPI"
                   ",B1_GRUPO"
                   ",B1_POSIPI"
                   ",B1_PE"
                   ",B1_MSBLQL"
                   ",B1_ATIVO"
                   ",B1_BITMAP"
                   ",B1_CODANT"  // 11 - código arternativo
                   ",B1_DESCRCP" // 12 - descrição completa
                   " FROM SB1010"
                   " WHERE B1_COD = :codigo"
                   " AND D_E_L_E_T_ =' ' ");
    qrypro.bindValue(":codigo",codp);
    if(!qrypro.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta qrypro");
        return;
    }
   qrypro.first();

    QString descricao;
    if(ui->checkBox_6->isChecked()){
        descricao = qrypro.value(12).toString().trimmed();
    }else{
        descricao = qrypro.value(0).toString().trimmed();
    }
    QString qtde_emba = qrypro.value(1).toString();
    QString peso_bruto = qrypro.value(2).toString();
    QString importado = qrypro.value(3).toString();
    double ipi = qrypro.value(4).toDouble();
    QString grupo = qrypro.value(5).toString();
    QString ncm = qrypro.value(6).toString();
    QString ncm4 = ncm.left(4);
    int B1_PE_p = qrypro.value(7).toInt();  // Prazo para iniciar a fabricação do produto (B1_PE)
    QString bloqueado = qrypro.value(8).toString();
    QString ativo = qrypro.value(9).toString();
    QString imagem_prod = qrypro.value(10).toString().left(7);

    ui->txt_desenho->setText(codp);
    ui->tw_produtos->setItem(contLi,0,new QTableWidgetItem(codp));  // código do produto  --------------------
    ui->tw_produtos->setItem(contLi,19,new QTableWidgetItem(grupo));  // grupo (série)  --------------------
    //![30]   //---------- LENDO VALORES DA TABELA SB1 ----------------//


    //![40]   //---------- OBTENDO A IMAGEM DO PRODUTO ----------------//
    QString usuario = m_dados.usuario;
    QString imagem_prod2=QString(imagem_prod+".jpg");
    QString imagemWeb = "C:/Users/"+usuario+"/Kap Componentes Eletricos Ltda/InfoKapQT - Documentos/prodimagem/"+imagem_prod2;
    QPixmap img(imagemWeb);
    ui->lb_imagem->setPixmap(img.scaled(150,150,Qt::KeepAspectRatio));
    //![40]   //---------- OBTENDO A IMAGEM DO PRODUTO ----------------//


    //![50]   //---------- VENDO SE O CÓDIGO DIGITADO É BLOQUEADO PARA VENDA ----------------//
    if(bloqueado == "1") {
        QString codp_a = qrypro.value(11).toString().trimmed().toUpper();
        if(codp_a.isEmpty()) {
            codp = cod_original;
        } else {
            codp = codp_a;
            QString m_obs = ui->txt_obs->toPlainText();
            ui->txt_obs->setText(m_obs + (m_obs.isEmpty() ? "" : " ") + cod_original + "=" + codp);
        }

        // Realiza nova consulta com o código alternativo/original
        QSqlQuery qrypro_b(ProtheusDB);
        qrypro_b.prepare("SELECT B1_DESC, "
                         "B1_QTDEMB, "
                         "B1_PESBRU, "
                         "B1_IMPORT, "
                         "B1_IPI, "
                         "B1_GRUPO, "
                         "B1_POSIPI, "
                         "B1_PE, "
                         "B1_MSBLQL, "
                         "B1_ATIVO, "
                         "B1_BITMAP,"
                         "B1_CODANT "
                         "FROM SB1010 "
                         "WHERE B1_COD = :codigo "
                         "AND D_E_L_E_T_ = ' ' ");
        qrypro_b.bindValue(":codigo",codp);
        if(!qrypro_b.exec()){
            QMessageBox::warning(this,"FALHA","Falha na consulta qrypro_b\n"+qrypro_b.lastError().text());
            return;
        }
        if(qrypro_b.next()){
            descricao = qrypro_b.value(0).toString();
            qtde_emba = qrypro_b.value(1).toString();
            peso_bruto = qrypro_b.value(2).toString();
            importado = qrypro_b.value(3).toString();
            ipi = qrypro_b.value(4).toDouble();
            grupo = qrypro_b.value(5).toString();
            ncm = qrypro_b.value(6).toString();
            ncm4 = ncm.left(4);
            B1_PE_p = qrypro_b.value(7).toInt();
            bloqueado = qrypro_b.value(8).toString();
            ativo = qrypro_b.value(9).toString();
            imagem_prod = qrypro_b.value(10).toString().trimmed().left(7);
        }
    }

        ui->txt_pesobruto->setText(peso_bruto);     // peso liquido (B1_PESBRUT)

        // --- [TRATAMENTO PARA PRODUTO NÃO CADASTRADO] ---
        if(qrypro.value(0).toString().isEmpty()) {
            codp = buscar_cod_kap().toUpper().simplified(); // Tenta buscar via função Kap
            if(codp.isEmpty()) {
                // CASO REALMENTE NÃO EXISTA NO CADASTRO NEM VIA BUSCA KAP
                // Preenche com valores padrão/vazios mas mantém o código digitado e quantidade
                ui->tw_produtos->setItem(contLi, 0, new QTableWidgetItem(cod_original));
                ui->tw_produtos->setItem(contLi, 1, new QTableWidgetItem(" "));
                ui->tw_produtos->setItem(contLi, 2, new QTableWidgetItem("Sem descrição"));
                ui->tw_produtos->setItem(contLi, 5, new QTableWidgetItem(ui->txt_qtde->text()));

                // Preenche o restante com "0"
                for(int col : {3,4,6,7,8,9,10,11,13,14,17})
                    ui->tw_produtos->setItem(contLi, col, new QTableWidgetItem("0"));

                ui->tw_produtos->setItem(contLi, 12, new QTableWidgetItem("N"));
                ui->tw_produtos->setItem(contLi, 15, new QTableWidgetItem("1"));
                ui->tw_produtos->setItem(contLi, 16, new QTableWidgetItem(" "));

                ui->tw_produtos->setItem(contLi, 18, new QTableWidgetItem(" "));
                ui->tw_produtos->setItem(contLi, 19, new QTableWidgetItem(" "));

                ui->tw_produtos->setRowHeight(contLi, 20);
                ui->txt_bloqueado->setText("S");
                ui->txt_bloqueado->setStyleSheet("background-color: rgb(255,125,100);");

                QMessageBox::information(this, "AVISO", "<h3>Produto não cadastrado. Adicionado como item genérico.</h3>");

                // Limpa e reseta o foco
                ui->txt_codP->clear();
                ui->txt_qtde->setFocus();
                //ui->tw_produtos->rootIndex() ;
                return; // Encerra aqui pois já inseriu manualmente
            } else {
                QSqlQuery qrypro_a(ProtheusDB);
                qrypro_a.prepare("SELECT B1_DESC, "
                                 "B1_QTDEMB, "
                                 "B1_PESBRU, "
                                 "B1_IMPORT, "
                                 "B1_IPI, "
                                 "B1_GRUPO, "
                                 "B1_POSIPI, "
                                 "B1_PE, "
                                 "B1_MSBLQL, "
                                 "B1_ATIVO, "
                                 "B1_BITMAP,"
                                 "B1_CODANT "
                                 "FROM SB1010 "
                                 "WHERE B1_COD = :codigo "
                                 "AND D_E_L_E_T_ = ' ' ");
                qrypro_a.bindValue(":codigo",codp);
                if(!qrypro_a.exec()){
                    QMessageBox::warning(this,"FALHA","Falha na consulta qrypro_a");
                    return;
                }
                if(qrypro_a.next()){
                    descricao = qrypro_a.value(0).toString();
                    qtde_emba = qrypro_a.value(1).toString();
                    peso_bruto = qrypro_a.value(2).toString();
                    importado = qrypro_a.value(3).toString();
                    ipi = qrypro_a.value(4).toDouble();
                    grupo = qrypro_a.value(5).toString();
                    ncm = qrypro_a.value(6).toString();
                    ncm4 = ncm.left(4);
                    B1_PE_p = qrypro_a.value(7).toInt();
                    bloqueado = qrypro_a.value(8).toString();
                    ativo = qrypro_a.value(9).toString();
                    imagem_prod=qrypro_a.value(10).toString().left(7);
                }

                QString m_obs = ui->txt_obs->toPlainText();
                if(m_obs.isEmpty()){
                    ui->txt_obs->setText(cod_original + "="+ codp);
                }else{
                    ui->txt_obs->setText(m_obs + " " + cod_original + "="+ codp);
                }
            }
        }

        ui->txt_desenho->setText(codp);
        ui->tw_produtos->setItem(contLi,0,new QTableWidgetItem(codp));  // código do produto  --------------------
        ui->tw_produtos->setItem(contLi,19,new QTableWidgetItem(grupo));  // grupo (série)  --------------------


     //![50]   //---------- VENDO SE O CÓDIGO DIGITADO É BLOQUEADO PARA VENDA - FIM ----------------//


    QString codcli=ui->txt_codCli->text();
    if(codcli.isNull()){
        ui->bt_ultimasVendas->setEnabled(false);
    }else{
        ui->bt_ultimasVendas->setEnabled(true);
    }
    QString loja=ui->txt_loja->text();
    ui->txt_qtde_embalagem->setText(qrypro.value(1).toString());

    //![60]   //----------  CÓDIGO DO PRODUTO NO CLIENTE - INÍCIO -------------------//
    QSqlQuery qrycc(ProtheusDB);
    qrycc.prepare("SELECT A7_CODCLI, "
                  "A7_DESCCLI "
                  "FROM SA7010 "
                  "WHERE A7_CLIENTE = :codcli "
                  "AND A7_LOJA = :loja "
                  "AND A7_PRODUTO = :codp "
                  "AND D_E_L_E_T_ = ' ' ");
    qrycc.bindValue(":codcli",codcli);
    qrycc.bindValue(":loja",loja);
    qrycc.bindValue(":codp",codp);
    if (!qrycc.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta qrycc\n"+ qrycc.lastError().text()) ;
        return;
    }
    if(qrycc.next()){
        ui->tw_produtos->setItem(contLi,1,new QTableWidgetItem(qrycc.value(0).toString()));  // código no cliente
    }
    ui->tw_produtos->setItem(contLi,2,new QTableWidgetItem(descricao));  // decrição do produto
      //![60]   //----------  CÓDIGO DO PRODUTO NO CLIENTE - FIM --------------------------//


      //![70]   //---------- CÓDIGO DO PRODUTO NO FORNECEDOR - INÍCIO -----------------------//
    QSqlQuery qry_no_for(ProtheusDB);
    qry_no_for.prepare("SELECT A5_CODPRF"
                       " FROM SA5010"
                       " WHERE A5_PRODUTO = :codigo"
                       " AND D_E_L_E_T_ = ' '");
    qry_no_for.bindValue(":codigo",codp);
    if(!qry_no_for.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta qry_no_for\n"+ qry_no_for.lastError().text()) ;
        return;
    }
    qry_no_for.first();
    QString cod_no_for = qry_no_for.value(0).toString().trimmed();
    cod_no_for.remove(" ");
    QString cod_no_fornecedor;
    if(codp != cod_no_for){
        cod_no_fornecedor = cod_no_for;
    }
    ui->tw_produtos->setItem(contLi,18,new QTableWidgetItem(cod_no_fornecedor));
     //![70]   //----------  CÓDIGO DO PRODUTO NO FORNECEDOR - FIM -----------------------//


     //![80]   //----------  PREÇO DE VENDA INÍCIO ------------------------//
    QSqlQuery qryvu(ProtheusDB);
    qryvu.prepare("SELECT DA1_PRCVEN "
                  "FROM DA1010 "
                  "WHERE DA1_CODPRO = :codp "
                  "AND DA1_CODTAB = '001' "
                  "AND D_E_L_E_T_ = ' '");
    qryvu.bindValue(":codp",codp);
    if(!qryvu.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta qryvu\n"+qryvu.lastError().text());
        return;
    }

    QString regiao;
    double precoUnit = 0;
    if(qryvu.next()){
        QString M07 = ui->cb_cf->currentText().replace(',','.');
        QString regiao = ui->txt_regiao->text().toUpper();

        if(regiao == "EXT"){
            QString DataHoje = QDate::currentDate().toString("yyyyMMdd");
            QSqlQuery qry_dolar(ProtheusDB);
            qry_dolar.prepare("SELECT M2_MOEDA2 "
                              "FROM SM2010 "
                              "WHERE M2_DATA = :datahoje "
                              "AND D_E_L_E_T_ = ' '");
            qry_dolar.bindValue(":datahoje",DataHoje);
            if(!qry_dolar.exec()){
                QMessageBox::warning(this,"FALHA","Falha na consulta qry_dolar\n"+qry_dolar.lastError().text());
                return;
            }
            if(qry_dolar.next()){
                if(qry_dolar.value(0).toString().isEmpty()){
                    QMessageBox::information(this,"AVISO","Não há registro do valor do dolar para a data de hoje");
                    return;
                }
                precoUnit = qryvu.value(0).toDouble() / qry_dolar.value(0).toDouble();
            }
        }else{
            // precoUnit = qryvu.value(0).toDouble() * (1+(M07.toDouble()/100));
            precoUnit = qryvu.value(0).toDouble();  //em 02/09/2025
        }
    }
    //![80]   //----------  PREÇO DE VENDA - FIM ------------------------------//




     //![90]   //----------  SE O PRODUTO É IMPORTADO - INÍCIO --------------//
    ui->txt_importado->setText(importado);
    ui->tw_produtos->setItem(contLi,12,new QTableWidgetItem (importado));
      //![90]   //----------  SE O PRODUTO É IMPORTADO - FIM ----------------//



      //![100]   //---------- OBTENDO O VALOR DO IPI - INÍCIO --------------------//
    if(regiao != "EXT"){
        QTableWidgetItem *ipi_a = new QTableWidgetItem(QString::number(ipi));
        ui->tw_produtos->setItem(contLi,7,ipi_a);
        ipi_a->setTextAlignment(Qt::AlignCenter);
    }else{
        ui->tw_produtos->setItem(contLi,7,new QTableWidgetItem("0"));  // EXT não tem IPI
    }
    //![100]   //---------- OBTENDO O VALOR DO IPI - FIM -----------------------------//


    //![110]   //---------- ROTINA DO DESCONTO - INÍCIO -----------------------------//
    //ui->txt_grupo->setText(qrypro.value(5).toString());
    m_grupo = qrypro.value(5).toString();
    QSqlQuery qrydesc1(ProtheusDB);
    qrydesc1.prepare("SELECT ACO_CODREG  "
                     "FROM ACO010 "
                     "WHERE ACO_CODCLI = :codcli "
                     "AND ACO_LOJA = :loja "
                     "AND D_E_L_E_T_ =' '");
    qrydesc1.bindValue(":codcli",codcli);
    qrydesc1.bindValue(":loja",loja);
    if(!qrydesc1.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta qrydesc1\n"+qrydesc1.lastError().text());
        return;
    }
    qrydesc1.first();
    QString ACO_CODREG = qrydesc1.value(0).toString().toUpper(); // tabela ACO010 Regra de desconto

    if(Desconto_excel > 0){
        Desconto = Desconto_excel;
    }else{
        Desconto = 0;
    }
    //QString tipo = ui->txt_tipo->text().toUpper();
    QString tipo = m_tipo;
    QString produto = ui->txt_codP->text().toUpper();

    if(Desconto_excel == 0){
        if(!ACO_CODREG.isEmpty() ){
            QSqlQuery qrydesc2(ProtheusDB);
            qrydesc2.prepare("SELECT ACP_PERDES "
                             "FROM ACP010 "
                             "WHERE ACP_CODREG = :ACO_CODREG "
                             "AND ACP_GRUPO = :grupo "
                             "AND D_E_L_E_T_ =' '");
            qrydesc2.bindValue(":ACO_CODREG",ACO_CODREG);
            qrydesc2.bindValue(":grupo",grupo);
            if(!qrydesc2.exec()){
                QMessageBox::warning(this,"FALHA","Falha na consulta qrydesc2\n"+qrydesc2.lastError().text());
                return;
            }
            qrydesc2.first();
            Desconto = qrydesc2.value(0).toDouble(); // tabela ACP010 Itens da regra de desconto - fornece o código do desconto

            if(Desconto == 0 ){
                QSqlQuery qrydesc2(ProtheusDB);
                qrydesc2.prepare("SELECT ACP_PERDES "
                                 "FROM ACP010 "
                                 "WHERE ACP_CODREG = :ACO_CODREG "
                                 "AND ACP_CODPRO = :produto "
                                 "AND D_E_L_E_T_ =' '");
                qrydesc2.bindValue(":ACO_CODREG",ACO_CODREG);
                qrydesc2.bindValue(":produto",produto);
                if(!qrydesc2.exec()){
                    QMessageBox::warning(this,"FALHA","Falha na consulta qrydesc2\n"+qrydesc2.lastError().text());
                    return;
                }
                qrydesc2.first();
                Desconto = qrydesc2.value(0).toDouble();
            }
        }
    }

    ui->tw_produtos->setItem(contLi,3,new QTableWidgetItem(QString::number(Desconto)));
    double precoUnitComDesconto=((100 - Desconto) / 100) * precoUnit;
    QTableWidgetItem *precoUnitComDesconto_a = new  QTableWidgetItem(brasilLocale.toString(precoUnitComDesconto,'f',2));
    ui->tw_produtos->setItem(contLi,4,precoUnitComDesconto_a);
    precoUnitComDesconto_a->setTextAlignment(Qt::AlignRight);
    //![110]   //---------- ROTINA DO DESCONTO - FIM -------------------------//


    //![120]   //---------- ROTINA DO NCM - INÍCIO ----------------------------//
    QTableWidgetItem *ncm_a = new QTableWidgetItem(ncm);
    ui->tw_produtos->setItem(contLi,9,ncm_a);
    ncm_a->setTextAlignment(Qt::AlignCenter);
     //![120]   //---------- ROTINA DO NCM - FIM ------------------------------//



      //![130]   //---------- ROTINA DO ST - INÍCIO -----------------------------//
    QString  uf = ui->txt_uf->text().toUpper();
    if(tipo == "S"){
        QSqlDatabase InfokapDB = QSqlDatabase::database("dbi");
        QSqlQuery qry_st(InfokapDB);
        qry_st.prepare("SELECT ST, NCM_ST4, UF_ST, Import "
                       "FROM RegraST_PorUF "
                       "WHERE NCM_ST4 = :ncm4 "
                       "AND UF_ST = :uf "
                       "AND Import = :importado");
        qry_st.bindValue(":ncm4",ncm4);
        qry_st.bindValue(":uf",uf);
        qry_st.bindValue(":importado",importado);
        if (!qry_st.exec()){
            QMessageBox::warning(this,"FALHA","Falha na consulta qry_st\n"+qry_st.lastError().text());
            return;
        }
        qry_st.first();
        QString aux = qry_st.value(0).toString();

        if(aux == "ATUAL"){
            QMessageBox::information(this,"AVISO","Solicitar valor do ST no departamento DAF ramal 230 e refazer o orçamento");
            return;
        }
        std::replace(aux.begin(),aux.end(),',','.');
        double st = aux.toDouble();
        if(st == 0){
            ui->tw_produtos->setItem(contLi,10,new QTableWidgetItem("0.00"));
        }else{
            ui->tw_produtos->setItem(contLi,10,new QTableWidgetItem(qry_st.value(0).toString()));
        }
    }else{
        QString st = "0.00";  // para cliente diferente de tipo = S
        ui->tw_produtos->setItem(contLi,10,new QTableWidgetItem(st));
    }
    //![130]   //---------- ROTINA DO ST - FIM --------------------------//


     //![140]   //---------- ROTINA DO ICMS - INICIO ---------------------//
    QString icms = "0";
    if(importado == "S"){
        if(uf!="SP"){
            icms = "4";
            //preço cheio = preço cheio * 0.82/0.96
        }else{
            icms = "18";
        }
    }else{
        if(uf == "MG" ||  uf == "PR" || uf == "RS" || uf == "RJ" || uf == "SC"  ){
            icms = "12";
        }else if (uf == "SP"){
            icms = "18";
        }else{
            icms = "7";
        }
    }
    ui->tw_produtos->setItem(contLi,17,new QTableWidgetItem(icms));
    //![140]   //---------- ROTINA DO ICMS - FIM ---------------------//


    //![150]   //---------- ESTOQUE LIVRE E EM TRÂMITE NÍVEL 0 - INÍCIO  --------------------------//
     estoqueS();
    //![150]   //---------- ESTOQUE LIVRE E EM TRÂMITE NÍVEL 0 - FIM  --------------------------//




    //![160]  // -------------- DETERMINAÇÃO DO PRAZO DE ENTREGA  - INÍCIO   --------------------------//

     int TPP_p = 0;          // Tempo  Para Produzir o Produto (em dias)
     int TPP_PC_c = 0;

     int prazo_p = 1;   //Prazo do produto quando estoque livre >0

     int e_livre = ui->txt_estoquelivre->text().toInt();
     int qtde_desejada = ui->txt_qtde->text().toInt();
     int prazo_final = 0;


     if(e_livre >= qtde_desejada) {
         B1_PE_p = 0;
         prazo_final = 1; // Pronta entrega
     } else {

         TPP_p = sub_tempo_TS();                 // busca o tempo em dias para o produto

         QSqlQuery qry_tem_estrutura(ProtheusDB);
         qry_tem_estrutura.prepare("SELECT G1_COD "
                                   "FROM SG1010 "
                                   "WHERE G1_COD = :codp "
                                   "AND G1_REVFIM = 'ZZZ' "
                                   "AND D_E_L_E_T_=' '");
         qry_tem_estrutura.bindValue(":codp",codp);
         if(!qry_tem_estrutura.exec()){
             QMessageBox::warning(this,"FALHA","Falha na consulta qry_tem_estrutura\n"+qry_tem_estrutura.lastError().text());
             return;
         }
         qry_tem_estrutura.first();
         int t = 0;
         while (qry_tem_estrutura.next()){
             t++;
         }

        int necessidade_produzir = 0;
        if(e_tramite + e_livre < 0){
            necessidade_produzir = qtde_desejada - (e_livre);
        }else{
            necessidade_produzir = qtde_desejada;
        }



        if(t == 0){
            prazo_final = B1_PE_p + TPP_p;
        }else{
            TPP_PC_c = calcularPrazoEstrutura(codp, necessidade_produzir);  // peça nível 1  - se 0, significa que tenho as peças
            prazo_final = prazo_p +  B1_PE_p + TPP_p + TPP_PC_c;
        }



         // Chamada única da função recursiva
        // int tempo_estrutura = calcularPrazoEstrutura(codp, necessidade_produzir);

         // Soma o tempo de montagem do produto pai (TPP_p) + gargalo da estrutura
        // prazo_final = prazo_p + TPP_p + tempo_estrutura;
        //  prazo_final = prazo_p +  B1_PE_p + TPP_p + TPP_PC_c + tempo_estrutura;
     }


     if(prazo_final < B1_PE_p){
         if((e_tramite + e_livre) > qtde.toInt()){
             B1_PE_p = int((B1_PE_p / 2) + 0.5); // não apagar
             //B1_PE_p = int((B1_PE_p) + 0.5);     // não apagar
             prazo_final = prazo_p + B1_PE_p;
         }
     }


     // Adiciona frete se necessário
     if(ui->cb_frete->currentText() == "CIF") prazo_final += 3;

     qDebug() << "B1_PE_p "+QString::number(B1_PE_p) << "prazo_p "+QString::number(prazo_p) << "prazo_final "+ QString::number(prazo_final) << "TPP_p "+QString::number(TPP_p)<< "TPP_PC_c "+QString::number(TPP_PC_c);

     ui->tw_produtos->setItem(contLi, 6, new QTableWidgetItem(QString::number(prazo_final)));


    //![160]   // -------------- DETERMINAÇÃO DO PRAZO DE ENTREGA  - FIM   --------------------------//



     //![170]   // -------------- QUANTIDADE X PREÇO UNITÁRIO  - INÍCIO --------------------------//
     ui->tw_produtos->setItem(contLi,11,new QTableWidgetItem(QString::number(precoUnit)));  // preço cheio
     QString aux = precoUnitComDesconto_a->text().remove('.').replace(',','.') ;
     double pu = aux.toDouble();
     double valorTotal = 0;

     valorTotal = (qtde.toDouble() * pu);

     QTableWidgetItem  *valorTotal_a = new QTableWidgetItem(brasilLocale.toString(valorTotal,'f',2));
     ui->tw_produtos->setItem(contLi,8,valorTotal_a);
     valorTotal_a->setTextAlignment(Qt::AlignRight);
     ui->tw_produtos->setItem(contLi,14,new QTableWidgetItem(peso_bruto));   // peso
     ui->tw_produtos->setItem(contLi,15,new QTableWidgetItem("1"));

     //![170]   // -------------- QUANTIDADE X PREÇO UNITÁRIO  - FIM --------------------------//


     //![180]   // -------------- TRATAMENTO PARA BLOQUEADOS E/OU INATIVOS - INÍCIO --------------------------//
     if(bloqueado == "1"){
         ui->txt_bloqueado->setText("S");
         ui->txt_bloqueado->setStyleSheet("*{background-color: rgb(255,125,100)}");
     }else{
         ui->txt_bloqueado->setText("N");
         ui->txt_bloqueado->setStyleSheet("*{background-color: rgb(255,255,255)}");
     }

     if(ativo == "S"){
         ui->txt_inativo->setText("S");
         ui->txt_inativo->setStyleSheet("*{background-color: rgb(255,255,255)}");
     }else{
         ui->txt_inativo->setText("N");
         ui->txt_inativo->setStyleSheet("*{background-color: rgb(255,125,100)}");
     }

     if(ativo == "N" || bloqueado == "1"){
         // ui->txt_inativo->setText("N"); // Verifique se a lógica de S/N está invertida conforme sua necessidade

         for(int i = 0; i < 14; i++){
             QTableWidgetItem *item = ui->tw_produtos->item(contLi, i);
             if(item){ // <--- PROTEÇÃO ESSENCIAL: Só executa se o item não for nulo
                 item->setBackground(Qt::red);
                 item->setForeground(Qt::white); // Melhora o contraste
             } else {
                 // Se o item não existia, criamos um vazio para não pular a cor
                 ui->tw_produtos->setItem(contLi, i, new QTableWidgetItem(""));
                 ui->tw_produtos->item(contLi, i)->setBackground(Qt::red);
             }
         }

         // Para a coluna 6,  colocar o "?" no valor do IPI
         if(ui->tw_produtos->item(contLi, 7)) {
             ui->tw_produtos->item(contLi, 7)->setText("?");
         } else {
             ui->tw_produtos->setItem(contLi, 7, new QTableWidgetItem("?"));
             ui->tw_produtos->item(contLi, 7)->setBackground(Qt::red);
         }

     } else {
         // Se estiver tudo OK, garante que o fundo seja branco (ou padrão)
         if(ui->tw_produtos->item(contLi, 0)) {
             ui->tw_produtos->item(contLi, 0)->setBackground(Qt::white);
         }
     }
     //![180]   // -------------- TRATAMENTO PARA BLOQUEADOS E/OU INATIVOS - FIM --------------------------//


    ui->tw_produtos->setItem(contLi,13,new QTableWidgetItem(imagem_prod2));
    ui->tw_produtos->setRowHeight(contLi,7);   //altura da linha
    // ui->tw_produtos->hideColumn(12);
    // ui->tw_produtos->hideColumn(13);
    // ui->tw_produtos->hideColumn(14);
    ui->tw_produtos->hideColumn(15);
    // ui->tw_produtos->hideColumn(16);

    Sub_somatorio_dos_precos_e_pesos() ;

    contLi++;

    ui->txt_codP_no_cli->clear();
    ui->txt_codP->clear();
    ui->txt_qtde->clear();
    ui->txt_qtde->setFocus();
    ui->tw_produtos->rootIndex() ;


}


int orcamento_novo::calcularPrazoEstrutura(QString codPai, int qtdeNecessariaPai, QStandardItemModel *model_estru) {
    int maiorTempoDosFilhos = 0;

    QLocale brasilLocale(QLocale::Portuguese,QLocale::Brazil);

    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
    QSqlQuery qry_estrutura(ProtheusDB);

    qry_estrutura.prepare(
        "SELECT "
        "   SG1.G1_COMP, "
        "   MAX(SG1.G1_QUANT) AS qtd_necessaria, "
        "   MAX(SB1.B1_PE) AS lead_time, "
        "   SB1.B1_DESC, "
        "   SUM(ISNULL(SB2.B2_QATU, 0) - ISNULL(SB2.B2_QEMP, 0)) AS SALDO_LIVRE, "
        "   (SELECT SUM(3600.0 / NULLIF(G2_LOTEPAD, 0)) "
        "    FROM SG2010 SG2 "
        "    WHERE SG2.G2_PRODUTO = SG1.G1_COMP "
        "    AND SG2.D_E_L_E_T_ = '') AS esforco_total_segundos, "
        "   (SELECT SUM(C2_QUANT - C2_QUJE) FROM SC2010 SC2 "
        "    WHERE C2_PRODUTO = SG1.G1_COMP "
        "    AND SC2.C2_QUJE < SC2.C2_QUANT "
        "    AND SC2.D_E_L_E_T_ = '') AS qtd_em_op, "
        "   (SELECT SUM(C7_QUANT - C7_QUJE) FROM SC7010 SC7 "
        "    WHERE C7_PRODUTO = SG1.G1_COMP "
        "    AND SC7.D_E_L_E_T_ = '') AS qtd_em_oc "
        "FROM SG1010 SG1 "
        "INNER JOIN SB1010 SB1 ON SB1.B1_COD = SG1.G1_COMP AND SB1.D_E_L_E_T_ = '' "
        "LEFT JOIN SB2010 SB2 ON SB2.B2_COD = SG1.G1_COMP "
        "   AND SB2.B2_LOCAL < '05' "
        "   AND SB2.D_E_L_E_T_ = '' "
        "WHERE SG1.G1_COD = :cod "
        "AND SG1.G1_REVFIM = 'ZZZ' "
        "AND SB1.B1_FANTASM = 'N' "
        "AND SB1.B1_TIPO IN ('MP', 'PI', 'PA') "
        "AND SG1.D_E_L_E_T_ = '' "
        "GROUP BY SG1.G1_COMP, SB1.B1_DESC"
        );


    qry_estrutura.bindValue(":cod", codPai.trimmed().toUpper());

    if (!qry_estrutura.exec()) {
        qDebug() << "ERRO SQL:" << qry_estrutura.lastError().text();
        return 0;
    }

    //qDebug() << "Query executada para:" << codPai << "Registros encontrados:" << qry_estrutura.size();

    if (!qry_estrutura.isActive()) {
        qDebug() << "Query Inativa!";
    }

    while (qry_estrutura.next()) {
        QString codFilho = qry_estrutura.value("G1_COMP").toString().trimmed();

        // REGRA: Ignorar Mão de Obra (MOD)
        if (codFilho.startsWith("MOD", Qt::CaseInsensitive)) continue;

        const double qtdNaEstrutura = qry_estrutura.value("qtd_necessaria").toDouble();
        const int leadTimeOriginal = qry_estrutura.value("lead_time").toInt();
        const int saldoLivre = qry_estrutura.value("SALDO_LIVRE").toInt();
        const double emOP = qry_estrutura.value("qtd_em_op").toDouble();
        const double emOC = qry_estrutura.value("qtd_em_oc").toDouble();
        const double emProcesso = emOP + emOC;
        qDebug() << codFilho  << qtdNaEstrutura << leadTimeOriginal << saldoLivre << emOP << emOC << emProcesso;

        int qtdeTotalNecessaria = qtdeNecessariaPai * qtdNaEstrutura;
        int tempoDesteRamoTotal = 0;
        int leadTimeEfetivo = 0;
        int tempoSub = 0;

        // REGRA: Se o saldo livre não cobre a necessidade, calculamos o atraso
        if (saldoLivre < qtdeTotalNecessaria) {
            int falta =  saldoLivre -  qtdeTotalNecessaria;
            leadTimeEfetivo = leadTimeOriginal;

            // --- APLICAÇÃO DA NORMA INTERNA (OP/OC) ---
           if (emProcesso >= static_cast<double>(falta) && leadTimeEfetivo > 0) {
               leadTimeEfetivo = std::max(1, leadTimeEfetivo / 2);
            }

            // RECURSÃO ÚNICA: Busca o tempo dos níveis abaixo
            tempoSub = calcularPrazoEstrutura(codFilho, static_cast<int>(falta), model_estru);


            // Soma cumulativa do ramo
            tempoDesteRamoTotal = leadTimeEfetivo + tempoSub;

            // --- ADICIONAR NA TABELA APENAS UMA VEZ ---
            if (model_estru) {
                QList<QStandardItem*> row;
                row << new QStandardItem(codPai);
                row << new QStandardItem(codFilho);
                row << new QStandardItem(qry_estrutura.value("B1_DESC").toString());
                row << new QStandardItem(brasilLocale.toString(qtdeTotalNecessaria));
                row << new QStandardItem(brasilLocale.toString(saldoLivre));
                row << new QStandardItem(brasilLocale.toString(emProcesso,'f',0));
                row << new QStandardItem(brasilLocale.toString(falta));
                if(emProcesso > 0){
                    row << new QStandardItem(QString::number(tempoDesteRamoTotal));
                }else{
                    row << new QStandardItem(QString::number(leadTimeOriginal));
                }
                model_estru->appendRow(row);
            }
        }

        // Caminho Crítico: O Pai espera pelo filho mais demorado
        if (tempoDesteRamoTotal > maiorTempoDosFilhos) {
            maiorTempoDosFilhos = tempoDesteRamoTotal;
        }

        // qDebug() << "CodFilho:" << codFilho << "LeadTime Original:" << leadTimeOriginal <<"LeadTime Efetivo "+QString::number(leadTimeEfetivo)
        //          << "Saldo:" << saldoLivre << "Em Processo:" << emProcesso
        //          << "Falta:" << (qtdeTotalNecessaria - saldoLivre)
        //          << "Prazo Acumulado Ramo:" << tempoDesteRamoTotal << "Qtde na estrutura "+QString::number(qtdNaEstrutura)
        //          <<  "qtdeTotalNecessaria "+ QString::number(qtdeTotalNecessaria) << "tempoSubEstrutura "+ QString::number(tempoSub);
    }

    return maiorTempoDosFilhos;
}



void orcamento_novo::on_txt_qtde_editingFinished()
{
    if(ui->txt_qtde->text().toInt() < 1){
        QMessageBox::information(this,"AVISO","A Quantidade não pode ser menor que 1");
        ui->txt_qtde->clear();
        ui->txt_qtde->setFocus();
        return;
    }

    if(ui->rb_cod_kap->isChecked()){
        ui->txt_codP->setFocus();
    }else if (ui->rb_cod_cliente->isChecked()){
        ui->txt_codP_no_cli->setFocus();
    }else{
        ui->txt_cod_concorrente->setFocus();
    }

    e_qtde = ui->txt_qtde->text();
}


void orcamento_novo::on_txt_nomeCli_editingFinished()
{
    if(ui->txt_nomeCli->text().isEmpty()){
        return;
    }
    cliloja();
}

void orcamento_novo::cliloja()
{
    ui->bt_importar_excel->setEnabled(true);
    ui->tw_cliloja->setRowCount(0); // Limpa a tabela instantaneamente

    QString regiao = m_dados.regiao.trimmed();
    QString vendedor = m_dados.vendedor.trimmed();
    QString uf_vendedor = m_dados.uf;
    QString nomecli = ui->txt_nomeCli->text().toUpper().trimmed();
    QString codcli = ui->txt_codCli->text().trimmed();
    QString cnpj_cli = ui->txt_cnpjCli->text().trimmed();

    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
    QSqlQuery qrycli(ProtheusDB);

    // 1. MONTAGEM DO SQL DINÂMICO
    QString sql = "SELECT A1_COD, A1_LOJA, A1_NREDUZ, A1_TIPO, A1_CGC, A1_END, "
                  "A1_BAIRRO, A1_MUN, A1_EST, A1_DDD, A1_TEL, A1_CONTATO, "
                  "A1_ULTCOM, A1_DTNASC, A1_EMLCOM, A1_VEND, A1_REGIAO, A1_RISCO, A1_MSBLQL "
                  "FROM SA1010 WHERE D_E_L_E_T_ = ' ' ";

    // Filtro Principal (Nome, Código ou CNPJ)
    if (!nomecli.isEmpty())   sql += "AND A1_NREDUZ LIKE :nome ";
    else if (!codcli.isEmpty()) sql += "AND A1_COD = :cod ";
    else if (!cnpj_cli.isEmpty()) sql += "AND A1_CGC = :cnpj ";
    else return; // Se tudo vazio, não faz nada

    // Filtros de Vendedor/Hierarquia
    if (vendedor != "000001") {
        if (vendedor == "0089") sql += "AND A1_REGIAO = :regiao ";
        else                   sql += "AND A1_EST = :uf ";
    }

    sql += " ORDER BY A1_COD, A1_LOJA";

    qrycli.prepare(sql);

    // Bind dos valores conforme a necessidade
    if (sql.contains(":nome"))   qrycli.bindValue(":nome", "%" + nomecli + "%");
    if (sql.contains(":cod"))    qrycli.bindValue(":cod", codcli);
    if (sql.contains(":cnpj"))   qrycli.bindValue(":cnpj", cnpj_cli);
    if (sql.contains(":regiao")) qrycli.bindValue(":regiao", regiao);
    if (sql.contains(":uf"))     qrycli.bindValue(":uf", uf_vendedor);

    if (!qrycli.exec()) {
        QMessageBox::warning(this, "Erro", "Falha na consulta Protheus.");
        return;
    }

    // 2. LÓGICA DE NEGÓCIO (CAMPOS DE TELA)
    ui->txt_prospect->setReadOnly(nomecli != "ORCAMENTO");
    ui->txt_codprospect->setReadOnly(nomecli != "ORCAMENTO");

    // 3. PREENCHIMENTO DA TABELA E CAMPOS
    int cLinhas = 0;
    ui->tw_cliloja->setColumnCount(12);

    // IMPORTANTE: qrycli.next() já move para o primeiro registro
    while (qrycli.next()) {
        ui->tw_cliloja->insertRow(cLinhas);

        // Dados da Tabela
        ui->tw_cliloja->setItem(cLinhas, 0, new QTableWidgetItem(qrycli.value(0).toString())); // Cod
        ui->tw_cliloja->setItem(cLinhas, 1, new QTableWidgetItem(qrycli.value(1).toString())); // Loja
        ui->tw_cliloja->setItem(cLinhas, 2, new QTableWidgetItem(qrycli.value(2).toString())); // Nome
        ui->tw_cliloja->setItem(cLinhas, 3, new QTableWidgetItem(qrycli.value(4).toString())); // CNPJ
        ui->tw_cliloja->setItem(cLinhas, 4, new QTableWidgetItem(qrycli.value(5).toString())); // Endereço
        ui->tw_cliloja->setItem(cLinhas, 5, new QTableWidgetItem(qrycli.value(6).toString())); // Bairro
        ui->tw_cliloja->setItem(cLinhas, 6, new QTableWidgetItem(qrycli.value(7).toString())); // Cidade
        ui->tw_cliloja->setItem(cLinhas, 7, new QTableWidgetItem(qrycli.value(8).toString())); // UF
        ui->tw_cliloja->setItem(cLinhas, 8, new QTableWidgetItem(qrycli.value(3).toString())); // Tipo
        ui->tw_cliloja->setItem(cLinhas, 9, new QTableWidgetItem(qrycli.value(15).toString())); // Vend
        ui->tw_cliloja->setItem(cLinhas, 10, new QTableWidgetItem(qrycli.value(17).toString())); // Risco
        QString bloq = qrycli.value(18).toString();
        QString bloq_st;
        if(bloq == "1"){
            bloq_st = "Sim";
        }else{
            bloq_st = "Não";
        }
        ui->tw_cliloja->setItem(cLinhas, 11, new QTableWidgetItem(bloq_st)); // Bloqueado s/n

        ui->tw_cliloja->setRowHeight(cLinhas, 20); // Altura mínima para ser clicável

        // Se for o primeiro registro, preenche os edits automaticamente
        if (cLinhas == 0) {
            preencherCamposCliente(qrycli);
            // Lógica do Tipo "S"
            if (qrycli.value(3).toString() == "S") {
                ui->cb_cf->setCurrentText("3,26");
                ui->cb_dll->setCurrentText("28");
            } else {
                ui->cb_cf->setCurrentText("0,00");
            }
        }
        cLinhas++;
    }

    // 4. ESTILIZAÇÃO DA TABELA
    configurarCabecalhosCliLoja();
    ui->txt_qtde->setFocus();
}

// Subfunção para não repetir código de preenchimento
void orcamento_novo::preencherCamposCliente(const QSqlQuery &q) {
    ui->txt_codCli->setText(q.value(0).toString());
    ui->txt_loja->setText(q.value(1).toString());
    ui->txt_nomeCli->setText(q.value(2).toString());
    ui->txt_tipo->setText(q.value(3).toString());
    m_tipo = q.value(3).toString();
    ui->txt_cnpjCli->setText(q.value(4).toString());
    //ui->txt_p_endereco->setText(q.value(5).toString());
    m_p_endereco = q.value(5).toString();
    //ui->txt_p_bairro->setText(q.value(6).toString());
    m_p_bairro = q.value(6).toString();
    //ui->txt_p_cidade->setText(q.value(7).toString());
    m_p_cidade = q.value(7).toString();
    ui->txt_uf->setText(q.value(8).toString());

    QString ddd = q.value(9).toString().trimmed().rightJustified(3, '0');
    ui->txt_ddd->setText(ddd);
   //ui->txt_ddd_original->setText(ddd);
    m_ddd_original = ddd;

    ui->txt_telefonecontato_1->setText(q.value(10).toString());
    //ui->txt_telefone_original->setText(q.value(10).toString());
    m_telefone_original = q.value(10).toString();

    ui->txt_contato_1->setText(q.value(11).toString());
    //ui->txt_contato_original->setText(q.value(11).toString());
    m_contato_original = q.value(11).toString();

    QString email = q.value(14).toString().trimmed();
    ui->txt_Emailcontato_1->setText(email);
   // ui->txt_email_original->setText(email);
    m_email_original = email;

    ui->txt_regiao->setText(q.value(16).toString());
}


void orcamento_novo::configurarCabecalhosCliLoja()
{
    // Definição de títulos
    QStringList cabecalhos = {
        "Código", "Loja", "Cliente", "CNPJ", "Endereço",
        "Bairro", "Cidade", "UF", "Tipo", "Vendedor", "Risco","Bloqueado"
    };
    ui->tw_cliloja->setHorizontalHeaderLabels(cabecalhos);

    // Ajuste de larguras (Organizado para fácil manutenção)
    QMap<int, int> larguras = {
        {0, 50}, {1, 15}, {2, 75}, {3, 100}, {4, 150},
        {5, 120}, {6, 130}, {7, 25}, {8, 25}, {9, 60}, {10, 50}, {11, 70}
    };

    for (auto it = larguras.begin(); it != larguras.end(); ++it) {
        ui->tw_cliloja->setColumnWidth(it.key(), it.value());
    }

    // Estilo e Comportamento
    ui->tw_cliloja->setStyleSheet("selection-background-color: lightBlue;");
    ui->tw_cliloja->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tw_cliloja->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tw_cliloja->verticalHeader()->setVisible(false);

    // Deixa a última coluna (Vendedor) esticar se sobrar espaço
    ui->tw_cliloja->horizontalHeader()->setStretchLastSection(true);
}


void orcamento_novo::on_txt_codP_cursorPositionChanged(int arg1, int arg2)
{
    Q_UNUSED(arg1);
    Q_UNUSED(arg2);

    QString codp = ui->txt_codP->text().toUpper().simplified();

    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
    QSqlQuery qry_bu(ProtheusDB);
    qry_bu.prepare("SELECT B1_COD FROM SB1010 WHERE B1_COD LIKE :codp "
                   "AND B1_TIPO = 'PA' AND D_E_L_E_T_ =' ' ");
    qry_bu.bindValue(":codp", "" + codp + "%");
    if(!qry_bu.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta qry_bu\n"+qry_bu.lastError().text());
        return;
    }
    qry_bu.first();
    ui->txt_codP_2->setText(qry_bu.value(0).toString().simplified());
}




QString orcamento_novo::buscar_cod_kap()
{
    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
    QSqlQuery qry_sa5(ProtheusDB);
    qry_sa5.prepare("SELECT A5_CODPRF, A5_PRODUTO "
                    "FROM SA5010 "
                    "WHERE A5_CODPRF = :codp "
                    "AND D_E_L_E_T_ = ' ' AND A5_SITU = 'C'");
    qry_sa5.bindValue(":codp",e_codp);
    if(!qry_sa5.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta qry_sa5\n"+qry_sa5.lastError().text());
        return 0;
    }
    qry_sa5.first();
    QString cod_kap = qry_sa5.value(1).toString().trimmed();
    return cod_kap;

}

void orcamento_novo::estoqueS()
{
    QString codp = e_codp;

    // 1. Verificação de segurança para obter o código
    if(codp.isEmpty()){
        int linha = ui->tw_produtos->currentRow();
        if (linha == -1) return; // Nenhuma linha selecionada, aborta.

        QTableWidgetItem *item = ui->tw_produtos->item(linha, 0);
        if (!item) return; // Célula vazia, aborta.

        codp = item->text().toUpper().trimmed();
    }

    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");


    QSqlQuery qryel(ProtheusDB);
    qryel.prepare("SELECT "
                  "Sum(B2_QATU), Sum(B2_QEMP), Sum(B2_SALPEDI) "
                  "FROM SB2010 "
                  "WHERE D_E_L_E_T_ = ' ' "
                  "AND B2_COD = :codp "
                  "AND B2_LOCAL < '05'");

    qryel.bindValue(":codp", codp);

    double B2_QATU = 0, B2_QEMP = 0;
    int v_tramite = 0;

    if(qryel.exec() && qryel.next()){
        B2_QATU = qryel.value(0).toDouble();
        B2_QEMP = qryel.value(1).toDouble();
        v_tramite = qryel.value(2).toInt();
    }

    // 3. Consulta SC6 otimizada
    QString data30 = QDate::currentDate().addDays(30).toString("yyyyMMdd");
    QSqlQuery qry_sc6(ProtheusDB);


    qry_sc6.prepare("SELECT Sum(C6_QTDVEN) "
                    "FROM SC6010 C6 "
                    "INNER JOIN SC5010 C5 ON C5.C5_NUM = C6.C6_NUM "
                    "WHERE C6.C6_PRODUTO = :codp "
                    "AND C6.C6_TES NOT IN ('511', '517', '509') "
                    "AND C6.D_E_L_E_T_ <> '*'"
                    "AND C6.C6_NOTA = ' ' "
                    "AND C5.C5_DTCLI < :data30 "
                    "AND C5.C5_TIPO = 'N' "
                    "AND C5.D_E_L_E_T_ <> '*'");

    qry_sc6.bindValue(":data30", data30);
    qry_sc6.bindValue(":codp", codp);

    double B2_QPEDVEN = 0;
    if(qry_sc6.exec() && qry_sc6.next()){
        B2_QPEDVEN = qry_sc6.value(0).toDouble();
    }

    // 4. Cálculos finais e atualização da UI
    e_livre = B2_QATU - B2_QEMP - B2_QPEDVEN;
    e_tramite = v_tramite;

    ui->txt_tramite->setText(QString::number(e_tramite));
    ui->txt_estoquelivre->setText(QString::number(e_livre, 'f', 0));
}

/*
int orcamento_novo::calcularPrazoEstrutura(QString codP, double qtdePedida)
{
    QString filial = "01";
    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");

    // 1. Consulta consolidada: Estrutura + Dados do Componente + Estoque
    QSqlQuery qry(ProtheusDB);
    qry.prepare(
        "SELECT G1.G1_COMP, G1.G1_QUANT, B1.B1_PE, B1.B1_TIPO, B1.B1_IMPORT, "
        " (SELECT SUM(B2_QATU - B2_QEMP - B2_RESERVA - B2_QPEDVEN) "
        "  FROM SB2010 SB2 "
        "  WHERE SB2.B2_COD = G1.G1_COMP AND SB2.B2_FILIAL = :filial "
        "  AND SB2.B2_LOCAL < '05' AND SB2.D_E_L_E_T_ = ' ') as SALDO_LIVRE, "
        " (SELECT SUM(B2_SALPEDI) FROM SB2010 SB2 "
        "  WHERE SB2.B2_COD = G1.G1_COMP AND SB2.B2_FILIAL = :filial "
        "  AND SB2.D_E_L_E_T_ = ' ') as EM_TRAMITE "
        "FROM SG1010 G1 "
        "INNER JOIN SB1010 B1 ON G1.G1_COMP = B1.B1_COD AND B1.B1_FILIAL = :filial "
        "WHERE G1.G1_COD = :produto AND G1.G1_FILIAL = :filial "
        "AND G1.G1_REVFIM = 'ZZZ' AND G1.D_E_L_E_T_ = ' ' AND B1.D_E_L_E_T_ = ' ' "
        "ORDER BY B1.B1_PE DESC"
        );

    qry.bindValue(":produto", codP);
    qry.bindValue(":filial", filial);

    if(!qry.exec()) return 0;

    int maiorPrazoDesteNivel = 0;

    while (qry.next()) {
        QString componente = qry.value("G1_COMP").toString().trimmed();
        double qtdNaEstrutura = qry.value("G1_QUANT").toDouble();
        int prazoLeadTime = qry.value("B1_PE").toInt();
        QString tipo = qry.value("B1_TIPO").toString().trimmed();
        double saldoLivre = qry.value("SALDO_LIVRE").toDouble();
        double emTramite = qry.value("EM_TRAMITE").toDouble();

        // Cálculo da necessidade bruta deste componente
        double necessidadeTotal = qtdNaEstrutura * qtdePedida;

        // Se não temos estoque suficiente para este componente
        if (saldoLivre < necessidadeTotal) {
            int prazoItem = 0;
            double falta = necessidadeTotal - saldoLivre;

            // CASO A: O que está em trâmite (OC/OP) resolve a falta?
            if (emTramite >= falta) {
                // Se já tem Ordem aberta, usamos um prazo reduzido (ex: metade ou fixo)
                // O ideal seria consultar a data real na SC7 ou SC2, mas usaremos sua lógica:
                prazoItem = (prazoLeadTime / 2);
            }
            // CASO B: É um Produto Intermediário (PI) ou Produzido (PA) - RECURSIVIDADE
            else if (tipo == "PI" || tipo == "PA") {
                // Chamada recursiva para abrir o próximo nível
                int prazoSubNivel = calcularPrazoEstrutura(componente, falta);
                prazoItem = prazoSubNivel + prazoLeadTime;
            }
            // CASO C: É Matéria-prima (MP) pura e não tem estoque nem trâmite
            else {
                prazoItem = prazoLeadTime;
            }

            // O prazo final do nível é sempre o do componente mais demorado (gargalo)
            if (prazoItem > maiorPrazoDesteNivel) {
                maiorPrazoDesteNivel = prazoItem;
            }
        }
    }

    return maiorPrazoDesteNivel;
}
*/
void orcamento_novo::on_bt_excluiritem_clicked()
{
    if(ui->tw_produtos->currentColumn()!=-1){
        ui->tw_produtos->removeRow(ui->tw_produtos->currentRow());
        contLi--;
        Sub_somatorio_dos_precos_e_pesos();
    }else{
        QMessageBox::information(this,"AVISO","Selecione um produto primeiro ....");
    }
}

void orcamento_novo::Sub_somatorio_dos_precos_e_pesos()
{

    //CÁLCULO DO SOMATÓRIO DOS PREÇOS E DOS PESOS - INÍCIO
    QLocale brasilLocale(QLocale::Portuguese,QLocale::Brazil);
    int qtdeItens = ui->tw_produtos->rowCount() ;

    QString aux,aux2;
    double w_valorTotal=0;
    double W_pesoLiquido=0;

    for(int i=0 ; i<qtdeItens; i++) {
        aux=ui->tw_produtos->item(i,4)->text().remove('.').replace(',','.');
        int qtdeiten = ui->tw_produtos->item(i,5)->text().toInt(); //  Qtde do produto orçado
        aux2=ui->tw_produtos->item(i,8)->text().remove('.').replace(',','.');
        w_valorTotal += aux2.toDouble();
        double peso = ui->tw_produtos->item(i,14)->text().toDouble() * qtdeiten;
        W_pesoLiquido += peso;
    }
    ui->txt_valorTotal->setText(brasilLocale.toString(w_valorTotal,'f',2));
    ui->txt_pesoTotal->setText(brasilLocale.toString(W_pesoLiquido,'f',4));
   // ui->txt_valorTotal_2->setText(QString::number(w_valorTotal)) ;
    //CÁLCULO DO SOMATÓRIO DOS PREÇOS E DOS PESOS - FIM
}

void orcamento_novo::on_bt_desenho_clicked()
{
    QSqlDatabase InfokapDB = QSqlDatabase::database("dbi");
    QString cod_prod = ui->txt_desenho->text().toUpper().trimmed();
    QSqlQuery qry_desenho(InfokapDB);
    qry_desenho.prepare("SELECT Codigo, NDesCG, NDes FROM Cadastro WHERE Codigo= :cod_prod ");
    qry_desenho.bindValue(":cod_prod",cod_prod);
    if(!qry_desenho.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta qry_desenho\n"+qry_desenho.lastError().text());
        return;
    }

    QString Num_desPri, Num_desProd;
    if(qry_desenho.next()){
        Num_desPri = qry_desenho.value(1).toString().trimmed();
        Num_desProd = qry_desenho.value(2).toString().trimmed();
    }

    QSqlQuery qry_doc(InfokapDB);
    if(Num_desPri.isEmpty()){
        qry_doc.prepare("SELECT PathPdf, NDoc FROM Documentos WHERE NDoc = :Num_desProd");
        qry_doc.bindValue(":Num_desProd",Num_desProd);
    }else{
        qry_doc.prepare("SELECT PathPdf, NDoc FROM Documentos WHERE NDoc = :Num_desPri");
        qry_doc.bindValue(":Num_desPri",Num_desPri);
    }
    if(!qry_doc.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consultaqry_doc\n"+qry_doc.lastError().text());
        return;
    }
    qry_doc.first();
    QString Path = qry_doc.value(0).toString();
    int Pathsize = Path.size();
    QString PathF = Path.right(Pathsize-1);
    QString PathL = PathF.left(Pathsize-2);
    QString y = "/";
    PathL.replace(2,1,y);

    if(Num_desPri.isEmpty() && Num_desProd.isEmpty()){
        QMessageBox::information(this,"Aviso","<h3>Não há desenho para este código</h3>");
        return;
    }

    QDesktopServices::openUrl(QUrl(PathL));
}




void orcamento_novo::on_cb_dll_currentTextChanged(const QString &arg1)
{
    Q_UNUSED(arg1);


    QLocale brasilLocale(QLocale::Portuguese,QLocale::Brazil);
    QString prazo = ui->cb_dll->currentText();
    QSqlDatabase InfokapDB = QSqlDatabase::database("dbi");
    QSqlQuery qry_cpcf(InfokapDB);
    qry_cpcf.prepare("SELECT CP, Por FROM CP WHERE CP = :prazo");
    qry_cpcf.bindValue(":prazo",prazo);
    if(!qry_cpcf.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta qry_cpcf\n"+qry_cpcf.lastError().text());
        return;
    }
    qry_cpcf.first();
    double npor = qry_cpcf.value(1).toDouble();
    ui->cb_cf->setCurrentText(brasilLocale.toString(npor,'f',2));
}


void orcamento_novo::on_bt_desconto_geral_clicked()
{
    int qtde_row = ui->tw_produtos->rowCount();
    if (qtde_row < 1) {
        QMessageBox::information(this, "AVISO", "<h3>Escolha um desconto para este orçamento</h3>");
        return;
    }

    QMessageBox::StandardButton resposta=QMessageBox::question(this,"","<h3>CONFIRMA IGUALAR OS DESCONTOS?</h3>",QMessageBox::Yes|QMessageBox::No);
    if(resposta == QMessageBox::No){
        return;
    }

    QLocale brasilLocale(QLocale::Portuguese,QLocale::Brazil);
    QString p1 =ui->tw_produtos->item(e_linha,3)->text() ;
    p1.replace(',', '.');

    for(int L = 0; L < qtde_row; ++L){
        ui->tw_produtos->setItem(L,3,new QTableWidgetItem(p1));
        double precoCheio = ui->tw_produtos->item(L,11)->text().toDouble() ;
        double precoUnitComDesconto=((100 - p1.toDouble()) / 100) * precoCheio;
        QTableWidgetItem *precoUnitComDesconto_a = new  QTableWidgetItem(brasilLocale.toString(precoUnitComDesconto,'f',2));
        ui->tw_produtos->setItem(L,4,precoUnitComDesconto_a);
        precoUnitComDesconto_a->setTextAlignment(Qt::AlignRight);
        double valorTotal = ui->tw_produtos->item(L,5)->text().toDouble() * precoUnitComDesconto;
        QTableWidgetItem  *valorTotal_a = new QTableWidgetItem(brasilLocale.toString(valorTotal,'f',2));
        ui->tw_produtos->setItem(L,8,valorTotal_a);
        valorTotal_a->setTextAlignment(Qt::AlignRight);
    }
    Sub_somatorio_dos_precos_e_pesos();
}


void orcamento_novo::on_bt_prazo_iguais_clicked()
{
    int qtde_row = ui->tw_produtos->rowCount();
    if (qtde_row < 1) {
        QMessageBox::information(this, "AVISO", "<h3>Escolha um prazo para este orçamento</h3>");
        return;
    }

    QMessageBox::StandardButton resposta=QMessageBox::question(this,"","<h3>CONFIRMA IGUALAR OS PRAZOS?</h3>",QMessageBox::Yes|QMessageBox::No);
    if(resposta == QMessageBox::No){
        return;
    }

    QString pz =ui->tw_produtos->item(e_linha,6)->text() ;

    for(int L = 0; L < qtde_row; ++L){
        ui->tw_produtos->setItem(L,6,new QTableWidgetItem(pz));
        QTableWidgetItem *pz_a = new  QTableWidgetItem(pz);
        ui->tw_produtos->setItem(L,6,pz_a);
        pz_a->setTextAlignment(Qt::AlignCenter);
    }
}


void orcamento_novo::on_tw_cliloja_cellClicked(int row, int column)
{
    Q_UNUSED(column);

    int linha = ui->tw_cliloja->currentRow();
    if (linha < 0) return;

    // Função auxiliar simples ou verificação direta
    auto getItemText = [&](int col) {
        QTableWidgetItem *it = ui->tw_cliloja->item(linha, col);
        return it ? it->text().trimmed() : QString("");
    };

    QString prospect = ui->txt_prospect->text().toUpper().trimmed();
    QString codcli = getItemText(0);
    QString lojacli = getItemText(1);

    if (prospect.isEmpty()) {
        QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
        QSqlQuery qrycli(ProtheusDB);
        qrycli.prepare("SELECT A1_COD, A1_LOJA, A1_NREDUZ, A1_TIPO, A1_CGC, A1_END, "
                    "A1_BAIRRO, A1_MUN, A1_EST, A1_DDD, A1_TEL, A1_CONTATO, "
                    "A1_ULTCOM, A1_DTNASC, A1_EMLCOM, A1_VEND, A1_REGIAO "
                    "FROM SA1010 "
                    "WHERE A1_COD = :cod AND A1_LOJA = :loja AND D_E_L_E_T_ = ' '");
        qrycli.bindValue(":cod", codcli);
        qrycli.bindValue(":loja", lojacli);

        if (qrycli.exec() && qrycli.next()) {
            ui->txt_codCli->setText(qrycli.value("A1_COD").toString());
            ui->txt_loja->setText(qrycli.value("A1_LOJA").toString());
            ui->txt_nomeCli->setText(qrycli.value("A1_NREDUZ").toString());
           // ui->txt_tipo->setText(qrycli.value("A1_TIPO").toString());
            ui->txt_cnpjCli->setText(qrycli.value("A1_CGC").toString());
            //ui->txt_p_endereco->setText(qrycli.value("A1_END").toString());
            //ui->txt_p_bairro->setText(qrycli.value("A1_BAIRRO").toString());
            //ui->txt_p_cidade->setText(qrycli.value("A1_MUN").toString());
            ui->txt_uf->setText(qrycli.value("A1_EST").toString());
            ui->txt_ddd->setText(qrycli.value("A1_DDD").toString().trimmed().rightJustified(3,'0'));
            ui->txt_telefonecontato_1->setText(qrycli.value("A1_TEL").toString());
            ui->txt_contato_1->setText(qrycli.value("A1_CONTATO").toString());
            ui->txt_Emailcontato_1->setText(qrycli.value("A1_EMLCOM").toString().trimmed());
            ui->txt_regiao->setText(qrycli.value("A1_REGIAO").toString());

            QString tipo = qrycli.value(3).toString();
            ui->cb_cf->setCurrentText(tipo == "S" ? "3,26" : "0,00");
            if(tipo == "S") ui->cb_dll->setCurrentText("28");
        }
    } else {
        QSqlDatabase infokapDB = QSqlDatabase::database("dbi");
        QSqlQuery qryprospct(infokapDB);

        qryprospct.prepare("SELECT CJ_PROSPE, CJ_LOJPRO, CJ_CONTATO, CJ_EMAIL, CJ_DDD, CJ_TEL, CJ_UF, CJ_TIPO, CJ_CGC "
                           "FROM Prospects "
                           "WHERE CJ_PROSPE = :cod AND CJ_LOJPRO = :loja");
        qryprospct.bindValue(":cod", codcli);
        qryprospct.bindValue(":loja", lojacli);

        if (qryprospct.exec() && qryprospct.next()) {
            ui->txt_cnpjCli->setText(qryprospct.value("CJ_CGC").toString());
            ui->txt_loja->setText(qryprospct.value("CJ_LOJPRO").toString());
            ui->txt_codprospect->setText(qryprospct.value("CJ_PROSPE").toString());
            ui->txt_contato_1->setText(qryprospct.value("CJ_CONTATO").toString());
            ui->txt_Emailcontato_1->setText(qryprospct.value("CJ_EMAIL").toString().trimmed());
            ui->txt_ddd->setText(qryprospct.value("CJ_DDD").toString().trimmed().rightJustified(3,'0'));
          //  ui->txt_ddd_original->setText(qryprospct.value("CJ_DDD").toString().trimmed().rightJustified(3,'0'));
            ui->txt_telefonecontato_1->setText(qryprospct.value("CJ_TEL").toString());
            ui->txt_uf->setText(qryprospct.value("CJ_UF").toString());
           // ui->txt_tipo->setText(qryprospct.value("CJ_TIPO").toString());
        }
    }

    ui->txt_qtde->setFocus();
}


void orcamento_novo::on_bt_editaritem_clicked()
{

    int contLi = ui->tw_produtos->currentRow();

    if(contLi != -1){
        Sessao_editar_item s; // Criando uma instância da struct

        // --- 1. POPULANDO A STRUCT COM DADOS DA TABELA ---
        s.codkap_item      = ui->tw_produtos->item(contLi, 0)->text();
        s.codnocli_item    = ui->tw_produtos->item(contLi, 1) ? ui->tw_produtos->item(contLi, 1)->text() : "";
        s.descricao_item   = ui->tw_produtos->item(contLi, 2)->text();
        s.desconto_item    = ui->tw_produtos->item(contLi, 3)->text();
        s.precoun_ititem   = ui->tw_produtos->item(contLi, 4)->text();
        s.qtdedo_item      = ui->tw_produtos->item(contLi, 5)->text();
        s.prazo_item       = ui->tw_produtos->item(contLi, 6)->text();
        s.ipi_item         = ui->tw_produtos->item(contLi, 7)->text();
        s.valortotal_item  = ui->tw_produtos->item(contLi, 8)->text();
        s.ncm_item         = ui->tw_produtos->item(contLi, 9)->text();
        s.st_item          = ui->tw_produtos->item(contLi, 10)->text();
        s.precocheio_item = ui->tw_produtos->item(contLi, 11)->text();
        s.import_item = ui->tw_produtos->item(contLi, 12)->text();
        s.cod_imagem_item = ui->tw_produtos->item(contLi, 13)->text();
        s.peso_item = ui->tw_produtos->item(contLi, 14)->text();

        s.codnoconc_item   = ui->tw_produtos->item(contLi, 16)->text();
        s.icms_item       = ui->tw_produtos->item(contLi, 17)->text();
        s.codnoforn_item   = ui->tw_produtos->item(contLi, 18)->text();

        s.uf_item = ui->txt_uf->text();
        s.regiao_item = ui->txt_regiao->text();


        // --- 2. CHAMANDO O FORMULÁRIO ---
        editar_item  *f_editar_item  = new editar_item(s, this);
        if(f_editar_item->exec() == QDialog::Accepted) {
            s = f_editar_item->dados_item;

            // Lambda para setar texto criando o item se ele não existir
            auto setTItem = [&](int r, int c, QString txt) {
                if(!ui->tw_produtos->item(r, c)) {
                    ui->tw_produtos->setItem(r, c, new QTableWidgetItem());
                }
                ui->tw_produtos->item(r, c)->setText(txt);
            };

            setTItem(contLi, 0, s.codkap_item);
            setTItem(contLi, 1, s.codnocli_item);
            setTItem(contLi, 2, s.descricao_item);
            setTItem(contLi, 3, s.desconto_item);
            setTItem(contLi, 4, s.precoun_ititem);
            setTItem(contLi, 5, s.qtdedo_item);
            setTItem(contLi, 6, s.prazo_item);
            setTItem(contLi, 7, s.ipi_item);
            setTItem(contLi, 8, s.valortotal_item);
            setTItem(contLi, 9, s.ncm_item);
            setTItem(contLi, 10, s.st_item);
            setTItem(contLi, 12, s.import_item);
            setTItem(contLi, 13, s.cod_imagem_item);
            setTItem(contLi, 14, s.peso_item);
            setTItem(contLi, 16, s.codnoconc_item);
            setTItem(contLi, 17, s.icms_item);
            setTItem(contLi, 18, s.codnoforn_item);

            ui->txt_uf->setText(s.uf_item);
            Sub_somatorio_dos_precos_e_pesos();
        }

        // Limpeza de memória
        delete f_editar_item;
    }
}


void orcamento_novo::on_tw_produtos_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    Q_UNUSED(row);

    e_linha= ui->tw_produtos->currentRow();
    QString codprod=ui->tw_produtos->item(e_linha,0)->text().toUpper() ;
    e_codp = codprod;
    ui->txt_desenho->setText(codprod);

    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
    QSqlQuery qrypro(ProtheusDB);
    qrypro.prepare("SELECT * FROM SB1010 "
                   "WHERE B1_COD = :codprod "
                   "AND D_E_L_E_T_ =' ' "
                   "ORDER BY B1_COD");
    qrypro.bindValue(":codprod",codprod);
    if(qrypro.exec()){
        qrypro.first();
        if(qrypro.value(1).toString()!=""){

            estoqueS();

            ui->txt_estoquelivre->setText(QString::number(e_livre));
            ui->txt_pesobruto->setText(qrypro.value(39).toString());
            ui->txt_qtde_embalagem->setText(qrypro.value(170).toString());

            QString importado = qrypro.value(92).toString();
            if(importado == "S"){
                ui->txt_importado->setText("S");
            }else{
                ui->txt_importado->setText("N");
            }

            QString bloqueado = qrypro.value(21).toString();
            if(bloqueado == "1"){
                ui->txt_bloqueado->setText("S");
                ui->txt_bloqueado->setStyleSheet("*{background-color: rgb(255,125,100)}");
            }else{
                ui->txt_bloqueado->setText("N");
                ui->txt_bloqueado->setStyleSheet("*{background-color: rgb(255,255,255)}");
            }

            QString ativo = qrypro.value(155).toString();
            if(ativo == "S"){
                ui->txt_inativo->setText("S");
                ui->txt_inativo->setStyleSheet("*{background-color: rgb(255,255,255)}");
            }else{
                ui->txt_inativo->setText("N");
                ui->txt_inativo->setStyleSheet("*{background-color: rgb(255,125,100)}");
            }

            //ui->txt_tramite->setText((qryel.value(5).toString()));
            ui->txt_tramite->setText(QString::number(e_tramite));

            QString usuario = m_dados.usuario;
            QString imagem_prod=qrypro.value(22).toString().left(7);
            QString imagem_prod2=QString(imagem_prod+".jpg");
            QString imagemWeb = "C:/Users/"+usuario+"/Kap Componentes Eletricos Ltda/InfoKapQT - Documentos/prodimagem/"+imagem_prod2;
            QPixmap img(imagemWeb);
            //QPixmap img("K:/"+imagem_prod2);
            ui->lb_imagem->setPixmap(img.scaled(150,150,Qt::KeepAspectRatio));

        }else {

            int linhaSemelhante = ui->tw_produtos->currentRow();
            QString imagem_prod = ui->tw_produtos->item(linhaSemelhante,13)->text();
            QString imagem_prod2=QString(imagem_prod+".jpg");
            QPixmap img("K:/"+imagem_prod2);
            ui->lb_imagem->setPixmap(img.scaled(150,150,Qt::KeepAspectRatio));

        }
        int linhaSemelhante = ui->tw_produtos->currentRow();
        //ui->txt_item_p_img_semelhante->setText(QString::number(linhaSemelhante));
    }else{
        QMessageBox::warning(this,"ERRO","Erro ao enserir novo produto ...."+qrypro.lastError().text());

    }
}


void orcamento_novo::on_tw_produtos_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(column);
    Q_UNUSED(row);

    on_bt_editaritem_clicked();
}


void orcamento_novo::on_bt_ultimasVendas_clicked()
{

    // 1. Validação inicial: verifica se há um item selecionado na tabela de produtos
    int linhaSelecionada = ui->tw_produtos->currentRow();
    if(linhaSelecionada == -1){
        QMessageBox::information(this, "AVISO","Escolha antes um item para visualizar as últimas vendas");
        return;
    }

    // 2. Coleta de dados da interface
    QString codcli = ui->txt_codCli->text();
    QString loja = ui->txt_loja->text();
    QString codproduto = ui->tw_produtos->item(linhaSelecionada, 0)->text().toUpper().trimmed();

    // 3. Consultas de suporte (Infokap - Data e Preço Ano Anterior)
    QSqlDatabase InfokapDB = QSqlDatabase::database("dbi");

    // Busca data de corte
    QSqlQuery qry_udata(InfokapDB);
    qry_udata.exec("SELECT Data_v_tp_aa FROM Data_vtpaa");
    qry_udata.first();
    QString udata_da_lista = qry_udata.value(0).toString(); // YYYYMMDD

    // Busca preço do ano anterior para cálculo de desconto
    QSqlQuery qry_tab_ap(InfokapDB);
    qry_tab_ap.prepare("SELECT PrecoLista_aa FROM Lista_ano_anterior WHERE Produto = :codprod");
    qry_tab_ap.bindValue(":codprod", codproduto);
    qry_tab_ap.exec();
    qry_tab_ap.first();
    double pap = qry_tab_ap.value(0).toDouble();

    // 4. Busca de Vendas no Protheus
    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
    QSqlQuery qry_uv(ProtheusDB);
    qry_uv.prepare("SELECT TOP 2500 C6_NUM, C6_DATFAT, C6_QTDVEN, C6_PRUNIT "
                   "FROM SC6010 "
                   "WHERE C6_CLI = :codcli AND C6_LOJA = :loja "
                   "AND C6_PRODUTO = :codproduto AND D_E_L_E_T_ =' ' "
                   "ORDER BY C6_DATFAT DESC");
    qry_uv.bindValue(":codcli", codcli);
    qry_uv.bindValue(":loja", loja);
    qry_uv.bindValue(":codproduto", codproduto);

    if(!qry_uv.exec() || !qry_uv.next()){
        QMessageBox::information(this,"Últimas Vendas","Não houve venda deste produto para este cliente.");
        return;
    }

    // 5. Criando o MODEL
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"Pedido", "Qtde", "Desc %", "Data", "Preço Unit."});

    do {
        int r = model->rowCount();
        QString rawDate = qry_uv.value(1).toString();
        double precoUnit = qry_uv.value(3).toDouble();
        QString dataFormatada = QDate::fromString(rawDate, "yyyyMMdd").toString("dd/MM/yyyy");

        model->setItem(r, 0, new QStandardItem(qry_uv.value(0).toString().trimmed()));
        model->setItem(r, 1, new QStandardItem(qry_uv.value(2).toString()));

        // Cálculo do desconto baseado no preço de lista do ano anterior
        if(!udata_da_lista.isEmpty() && rawDate >= udata_da_lista && pap > 0){
            double desconto = (1 - (precoUnit / pap)) * 100;
            model->setItem(r, 2, new QStandardItem(QString::number(desconto, 'f', 2) + "%"));
        } else {
            model->setItem(r, 2, new QStandardItem("-"));
        }

        model->setItem(r, 3, new QStandardItem(dataFormatada));
        model->setItem(r, 4, new QStandardItem(QString::number(precoUnit, 'f', 2)));

    } while (qry_uv.next());

    // --- 6. CRIAÇÃO DA JANELA FLUTUANTE (QDialog) ---

    QDialog *janelaVendas = new QDialog(this);
    janelaVendas->setWindowTitle("Histórico do Produto: " + codproduto);
    janelaVendas->resize(650, 450);
    janelaVendas->setAttribute(Qt::WA_DeleteOnClose); // Libera memória ao fechar

    QTableView *tabelaVendas = new QTableView(janelaVendas);
    tabelaVendas->setModel(model);
    tabelaVendas->setSelectionBehavior(QAbstractItemView::SelectRows);
    tabelaVendas->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabelaVendas->verticalHeader()->setVisible(false); // Esconde números das linhas
    tabelaVendas->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Layout para organizar a tabela dentro da janela
    QVBoxLayout *layout = new QVBoxLayout(janelaVendas);
    layout->addWidget(tabelaVendas);
    janelaVendas->setLayout(layout);

    // Exibe a janela
    janelaVendas->show();
    janelaVendas->raise();
    janelaVendas->activateWindow();

        // // 1. Controle de Visibilidade (Lógica simplificada)
        // if(ui->tw_produtos->currentRow() == -1){
        //     QMessageBox::information(this, "AVISO","Escolha antes um item para visualizar as últimas vendas");
        //     return;
        // }

        // if (vi == "0") { // Se já estiver visível, esconde
        //     tv_ultimasVendas->setVisible(false);
        //     ui->bt_excluiritem->setVisible(true);
        //     ui->bt_editaritem->setVisible(true);
        //     ui->bt_abrirOP->setVisible(true);
        //     vi = "1";
        //     return;
        // }

        // // Configuração de exibição
        // tv_ultimasVendas->setVisible(true);

        // vi = "0";
        // ui->bt_excluiritem->setVisible(false);
        // ui->bt_editaritem->setVisible(false);
        // ui->bt_abrirOP->setVisible(false);

        // // 2. Coleta de dados da interface
        // QString codcli = ui->txt_codCli->text();
        // QString loja = ui->txt_loja->text();
        // int linha = ui->tw_produtos->currentRow();
        // QString codproduto = ui->tw_produtos->item(linha,0)->text().toUpper();

        // // 3. Consultas de suporte (Data e Preço Ano Anterior)
        // QSqlDatabase InfokapDB = QSqlDatabase::database("dbi");
        // QSqlQuery qry_udata(InfokapDB);
        // qry_udata.exec("SELECT Data_v_tp_aa FROM Data_vtpaa");
        // qry_udata.first();
        // QString udata_da_lista = qry_udata.value(0).toString(); // YYYYMMDD

        // QSqlQuery qry_tab_ap(InfokapDB);
        // qry_tab_ap.prepare("SELECT PrecoLista_aa FROM Lista_ano_anterior WHERE Produto = :codprod");
        // qry_tab_ap.bindValue(":codprod", codproduto);
        // qry_tab_ap.exec();
        // qry_tab_ap.first();
        // double pap = qry_tab_ap.value(0).toDouble();

        // // 4. Busca de Vendas no Protheus
        // QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
        // QSqlQuery qry_uv(ProtheusDB);
        // qry_uv.prepare("SELECT TOP 2500 C6_NUM, C6_DATFAT, C6_QTDVEN, C6_PRUNIT "
        //                "FROM SC6010 "
        //                "WHERE C6_CLI = :codcli AND C6_LOJA = :loja "
        //                "AND C6_PRODUTO = :codproduto AND D_E_L_E_T_ =' ' "
        //                "ORDER BY C6_DATFAT DESC");
        // qry_uv.bindValue(":codcli", codcli);
        // qry_uv.bindValue(":loja", loja);
        // qry_uv.bindValue(":codproduto", codproduto);

        // if(!qry_uv.exec() || !qry_uv.next()){
        //     QMessageBox::information(this,"Últimas Vendas","Não houve venda deste produto.");
        //     // Reverter visibilidade se não achar nada
        //     on_bt_ultimasVendas_clicked();
        //     return;
        // }

        // // 5. Criando o MODEL (A parte "inteligente")
        // QStandardItemModel *model = new QStandardItemModel(this);
        // model->setHorizontalHeaderLabels({"Nº Pedido", "Qtde", "Desconto %", "Data", "Preço Unit."});

        // do {
        //     int r = model->rowCount();
        //     QString rawDate = qry_uv.value(1).toString();
        //     double precoUnit = qry_uv.value(3).toDouble();

        //     // Formata data de YYYYMMDD para DD/MM/YYYY
        //     QString dataFormatada = QDate::fromString(rawDate, "yyyyMMdd").toString("dd/MM/yyyy");

        //     model->setItem(r, 0, new QStandardItem(qry_uv.value(0).toString()));
        //     model->setItem(r, 1, new QStandardItem(qry_uv.value(2).toString()));

        //     // Cálculo do desconto
        //     if(rawDate >= udata_da_lista && pap > 0){
        //         double desconto = (1 - (precoUnit / pap)) * 100;
        //         model->setItem(r, 2, new QStandardItem(QString::number(desconto, 'f', 2)));
        //     } else {
        //         model->setItem(r, 2, new QStandardItem("-"));
        //     }

        //     model->setItem(r, 3, new QStandardItem(dataFormatada));
        //     model->setItem(r, 4, new QStandardItem(QString::number(precoUnit, 'f', 2)));

        // } while (qry_uv.next());

        // // 6. Aplica o model na View e formata
        //  model->setHorizontalHeaderLabels({"Pedido", "Qtde", "Desc %", "Data", "Preço"});

        // // Ajuste de colunas
        //  tv_ultimasVendas->setModel(model);

        //  // Ajustes finos de exibição
        //  tv_ultimasVendas->setSelectionBehavior(QAbstractItemView::SelectRows);
        //  tv_ultimasVendas->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        //  // Centralizar a tabela na tela atual:
        //  int x = (this->width() - 600) / 4;  // Largura da janela menos largura da tabela / 2
        //  int y = (this->height() - 600) / 2; // Altura da janela menos altura da tabela / 2

        //  tv_ultimasVendas->setGeometry(x, y, 400, 600);
        //  tv_ultimasVendas->raise(); // Traz a tabela para a frente de todos os outros botões
        //  tv_ultimasVendas->show();

}


void orcamento_novo::on_bt_oquecomprou_clicked()
{

    // 1. Validação inicial
        if(ui->txt_codCli->text().isEmpty()){
        QMessageBox::information(this, "AVISO","<h3>Você não informou o código do cliente</h3>");
        return;
    }

    // 3. Preparação dos dados (Pulei o passo 2 do Toggle pois o Dialog resolve o fechamento)
    QString codcli = ui->txt_codCli->text();
    QString loja = ui->txt_loja->text();
    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
    QSqlQuery qry_uv(ProtheusDB);

    qry_uv.prepare("SELECT TOP 250 "
                   "SC5.C5_NUM, SC6.C6_DATFAT, SC6.C6_PRODUTO, SC6.C6_QTDVEN, SC6.C6_PRUNIT, "
                   "SB1.B1_IMPORT, SE4.E4_COND "
                   "FROM SC5010 SC5 "
                   "INNER JOIN SC6010 SC6 ON SC5.C5_NUM = SC6.C6_NUM "
                   "LEFT JOIN SB1010 SB1 ON SB1.B1_COD = SC6.C6_PRODUTO AND SB1.D_E_L_E_T_ = ' ' "
                   "LEFT JOIN SE4010 SE4 ON SE4.E4_CODIGO = SC5.C5_CONDPAG AND SE4.D_E_L_E_T_ = ' ' "
                   "WHERE SC6.C6_CLI = :codcli AND SC6.C6_LOJA = :loja "
                   "AND SC5.C5_TIPO = 'N' AND SC6.C6_TES NOT IN ('511','509','517') "
                   "AND SC5.D_E_L_E_T_ = ' ' AND SC6.D_E_L_E_T_ = ' ' "
                   "ORDER BY SC6.C6_DATFAT DESC");

    qry_uv.bindValue(":codcli", codcli);
    qry_uv.bindValue(":loja", loja);

    if(!qry_uv.exec()){
        QMessageBox::warning(this,"FALHA","Erro no banco: " + qry_uv.lastError().text());
        return;
    }

    // 4. Criando o Model
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"Nº Pedido", "Data", "Produto", "Import.", "Qtde", "Preço Unit.", "Cond. Pgto"});

    int rows = 0;
    while(qry_uv.next()){
        QString dataFormatada = QDate::fromString(qry_uv.value("C6_DATFAT").toString(), "yyyyMMdd").toString("dd/MM/yyyy");
        model->setItem(rows, 0, new QStandardItem(qry_uv.value("C5_NUM").toString().trimmed()));
        model->setItem(rows, 1, new QStandardItem(dataFormatada));
        model->setItem(rows, 2, new QStandardItem(qry_uv.value("C6_PRODUTO").toString().trimmed()));
        model->setItem(rows, 3, new QStandardItem(qry_uv.value("B1_IMPORT").toString().trimmed()));
        model->setItem(rows, 4, new QStandardItem(qry_uv.value("C6_QTDVEN").toString()));
        model->setItem(rows, 5, new QStandardItem(QString::number(qry_uv.value("C6_PRUNIT").toDouble(), 'f', 2)));
        model->setItem(rows, 6, new QStandardItem(qry_uv.value("E4_COND").toString().trimmed()));
        rows++;
    }

    if(rows == 0){
        QMessageBox::information(this,"O que comprou","Não houve venda para este cliente");
        return;
    }

     // 1. Criar o diálogo (Alocado dinamicamente para não fechar ao sair da função)

    QDialog *janelaPopUp = new QDialog(this);
    janelaPopUp->setWindowTitle("Histórico de Compras - Cliente: " + codcli + " Loja: "+ loja);
    janelaPopUp->resize(850, 500);
    // Garante que a memória seja liberada ao fechar o X
    janelaPopUp->setAttribute(Qt::WA_DeleteOnClose);

    // 2. Criar uma nova View para esta janela (Melhor que mover a global)
    QTableView *tabela = new QTableView(janelaPopUp);
    tabela->setModel(model);
    tabela->setSelectionBehavior(QAbstractItemView::SelectRows);
    tabela->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabela->verticalHeader()->setDefaultSectionSize(25);
    tabela->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    tabela->setColumnWidth(2, 180); // Descrição do produto

    // 3. Organizar no Layout (Faz a tabela ocupar todo o espaço do Dialog)
    QVBoxLayout *layout = new QVBoxLayout(janelaPopUp);
    layout->addWidget(tabela);
    janelaPopUp->setLayout(layout);

    // 4. Exibir de forma não-modal (permite continuar usando o sistema com ela aberta)
    janelaPopUp->show();
    janelaPopUp->raise();
    janelaPopUp->activateWindow();


    /*

    // 1. Validação inicial
    if(ui->txt_codCli->text().isEmpty()){
        QMessageBox::information(this, "AVISO","<h3>Você não informou o código do cliente</h3>");
        return;
    }

    // 2. Lógica de Visibilidade (Toggle)
    if (tv_oqueComprou->isVisible()) {
        tv_oqueComprou->setVisible(false);
        ui->bt_excluiritem->setVisible(true);
        ui->bt_editaritem->setVisible(true);
        ui->bt_abrirOP->setVisible(true);
        vi = "1";
        return;
    }

    // 3. Preparação dos dados
    QString codcli = ui->txt_codCli->text();
    QString loja = ui->txt_loja->text();
    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
    QSqlQuery qry_uv(ProtheusDB);

    qry_uv.prepare("SELECT TOP 250 "
                   "SC5.C5_NUM, SC6.C6_DATFAT, SC6.C6_PRODUTO, SC6.C6_QTDVEN, SC6.C6_PRUNIT, "
                   "SB1.B1_IMPORT, SE4.E4_COND "
                   "FROM SC5010 SC5 "
                   "INNER JOIN SC6010 SC6 ON SC5.C5_NUM = SC6.C6_NUM "
                   "LEFT JOIN SB1010 SB1 ON SB1.B1_COD = SC6.C6_PRODUTO AND SB1.D_E_L_E_T_ = ' ' "
                   "LEFT JOIN SE4010 SE4 ON SE4.E4_CODIGO = SC5.C5_CONDPAG AND SE4.D_E_L_E_T_ = ' ' "
                   "WHERE SC6.C6_CLI = :codcli AND SC6.C6_LOJA = :loja "
                   "AND SC5.C5_TIPO = 'N' AND SC6.C6_TES NOT IN ('511','509','517') "
                   "AND SC5.D_E_L_E_T_ = ' ' AND SC6.D_E_L_E_T_ = ' ' "
                   "ORDER BY SC6.C6_DATFAT DESC");

    qry_uv.bindValue(":codcli", codcli);
    qry_uv.bindValue(":loja", loja);

    if(!qry_uv.exec()){
        QMessageBox::warning(this,"FALHA","Erro no banco: " + qry_uv.lastError().text());
        return;
    }

    // 4. Criando o Model (QStandardItemModel)
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"Nº Pedido", "Data", "Produto", "Import.", "Qtde", "Preço Unit.", "Cond. Pgto"});

    int rows = 0;
    while(qry_uv.next()){
        // Formatação de Data Nativa
        QString dataFormatada = QDate::fromString(qry_uv.value("C6_DATFAT").toString(), "yyyyMMdd").toString("dd/MM/yyyy");

        model->setItem(rows, 0, new QStandardItem(qry_uv.value("C5_NUM").toString().trimmed()));
        model->setItem(rows, 1, new QStandardItem(dataFormatada));
        model->setItem(rows, 2, new QStandardItem(qry_uv.value("C6_PRODUTO").toString().trimmed()));
        model->setItem(rows, 3, new QStandardItem(qry_uv.value("B1_IMPORT").toString().trimmed()));
        model->setItem(rows, 4, new QStandardItem(qry_uv.value("C6_QTDVEN").toString()));
        model->setItem(rows, 5, new QStandardItem(QString::number(qry_uv.value("C6_PRUNIT").toDouble(), 'f', 2)));
        model->setItem(rows, 6, new QStandardItem(qry_uv.value("E4_COND").toString().trimmed()));

        rows++;
    }

    // 5. Verificar se encontrou algo
    if(rows == 0){
        QMessageBox::information(this,"O que comprou","Não houve venda para este cliente");
        return;
    }

    // 6. Configurar e exibir a View centralizada
    tv_oqueComprou->setModel(model);

    // Centralização dinâmica
    // int largura = 750; // Um pouco mais largo que o anterior por causa das colunas extras
    // int altura = 500;
    // int x = (this->width() - largura) / 10;
    // int y = (this->height() - altura) / 2;

    // tv_oqueComprou->setGeometry(x, y, largura, altura);
    tv_oqueComprou->setSelectionBehavior(QAbstractItemView::SelectRows);
    tv_oqueComprou->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tv_oqueComprou->verticalHeader()->setDefaultSectionSize(25); // Altura da linha melhorada

    // Ajuste das colunas
    tv_oqueComprou->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    tv_oqueComprou->setColumnWidth(2, 120); // Coluna de produto maior

    // UI Feedback

    // 1. Criar o diálogo
    QDialog *janelaPopUp = new QDialog(this);
    janelaPopUp->setWindowTitle("O que o cliente comprou");
    janelaPopUp->resize(750, 500);

    // 2. Colocar sua tabela dentro dele usando um Layout
    QVBoxLayout *layout = new QVBoxLayout(janelaPopUp);
    layout->addWidget(tv_oqueComprou);
    janelaPopUp->setLayout(layout);

    // 3. Exibir
    janelaPopUp->show();

    // tv_oqueComprou->show();
    // tv_oqueComprou->raise();

    ui->bt_excluiritem->setVisible(false);
    ui->bt_editaritem->setVisible(false);
    ui->bt_abrirOP->setVisible(false);
    //vi = "0";

*/
}

void orcamento_novo::keyPressEvent(QKeyEvent* ke)
{
    if(ke->key()== Qt::Key_Escape){
        return;
    }

}

void orcamento_novo::on_bt_salvar_clicked()
{
    QString cab;
    // gravação do cabeçalho - início
    // --- Validação de campos obrigatórios -------
    if (!validarCamposObrigatorios()) {
        return;
    }

    int qtde_de_itens = ui->tw_produtos->rowCount();
    if (qtde_de_itens < 1) {
        QMessageBox::information(this, "AVISO", "<h3>Você não informou itens para este orçamento</h3>");
        return;
    }

    // --- Conexão com banco ---
    QSqlDatabase infokapDB = QSqlDatabase::database("dbi");



    // --- Preparação dos dados ---
    QString M01 = ui->txt_codCli->text().left(6);
    QString M02 = ui->txt_loja->text().left(2);
    QString M03 = ui->txt_codprospect->text().trimmed();
    QString M04 = QDate::currentDate().toString("dd/MM/yyyy");
    QString M05 = QTime::currentTime().toString("hh:mm:ss");
    QString M06 = ui->cb_atendente->currentText().toUpper();

    QString aux_cf = ui->cb_cf->currentText();
    aux_cf.replace(",", ".");
    QString M07 = aux_cf;

    QString M08 = ui->cb_dll->currentText().left(20);
    double M09 = ui->cb_cf->currentText().toDouble();
    QString M10 = ui->checkBox->isChecked() ? "1" : "0";   // Valores mínimos p/ pgto à vista
    QString M11 = ui->txt_obs->toPlainText().left(255).simplified();
    QString M12 = ui->txt_desconto->text().simplified();
    QString M13 = ui->checkBox_2->isChecked() ? "1" : "0"; // Enviar ficha cadastral
    QString M14 = ui->checkBox_3->isChecked() ? "1" : "0"; // Relação de bancos

    QString M15 = ui->txt_contato_1->text().simplified().left(15);
    QString M16 = ui->txt_Emailcontato_1->text().toLower().simplified().left(80);
    QString M17 = ui->txt_ddd->text().trimmed().rightJustified(3, '0');
    QString M18 = ui->txt_telefonecontato_1->text().simplified().left(15);

    QString M19 = m_p_endereco.simplified().left(100).replace("'", ".");
    QString M20 = m_p_bairro.simplified().left(100).replace("'", ".");
    QString M21 = m_p_cidade.simplified().left(50).replace("'", ".");
    QString M22 = ui->txt_uf->text().left(2).toUpper();

    QString M23 = "N";
    QString M24 = "0";
    QString M25 = "";
    QString M26 = "";
    QString M27 = QDate::currentDate().toString("dd/MM/yyyy");
    QString M28 = ui->txt_celularcontato_1->text().simplified();

    QString M31 = ui->checkBox_4->isChecked() ? "1" : "0"; // Mostrar valor total
    QString M32 = ui->checkBox_5->isChecked() ? "1" : "0"; // Não enviar orçamento
    QString M33 = ui->cb_frete->currentText().left(1);

    QString M34, M38;
    if (M03.isEmpty()) {
        M34 = ui->txt_nomeCli->text().simplified().left(20);
    } else {
        M34 = "ORCAMENTO";
        M38 = ui->txt_prospect->text().toUpper().left(20);
        M01 = "000001";
        M02 = "01";
    }

    QString M35 = ui->txt_cnpjCli->text().left(14);
    QString M36 = ui->txt_tipo->text().left(1);
    QString M37 = ui->txt_valorTotal->text().remove(".").replace(",", ".");
    QString M39 = "";
    QString M40 = ui->txt_regiao->text().trimmed();
    QString M41 = ui->txt_frete->text().remove(".").replace(",", ".");

    // bool witem = true;
    // cab = "S";


    // Inicia a transação para garantir que grava "TUDO ou NADA"
    if (!infokapDB.transaction()) {
        QMessageBox::critical(this, "Erro", "Não foi possível iniciar a gravação do orçamento.");
        return;
    }

    try {

        // Pegar último número de cotação
        QSqlQuery qryLast(infokapDB);
        qryLast.prepare("SELECT MAX(NDaCotacao) FROM Cotacoes");
        int MMU = 1;
        if (qryLast.exec() && qryLast.next()) {
            MMU = qryLast.value(0).toInt() + 1;
        }


        // --- Gravação no banco ---
        QSqlQuery qry(infokapDB);
        qry.prepare(R"(
        INSERT INTO Cotacoes (
            NDaCotacao, Cliente, A1_LOJA, CJ_PROSPE, Data, horario, Atendente,
            CustoFinanceiro, DDL, ValorDDL, MostrarDDL, OBS, Desconto,
            EnviarFichaCadastral, Semail, Contato, Email, Telefone,
            Endereco, Bairro, Cidade, UF, VP, QV, NCotCli, NpedidoCli,
            DataParaCompra, CelularCli, Imprime2, NaoEnviar, Frete,
            Fantasia, CNPJ, A1_TIPO, ValorTot, PROSP, TP, Regiao, Valor_frete
        ) VALUES (
            :MMU, :M01, :M02, :M03, :M04, :M05, :M06,
            :M07, :M08, :M09, :M10, :M11, :M12,
            :M13, :M14, :M15, :M16, :M17M18,
            :M19, :M20, :M21, :M22, :M23, :M24, :M25, :M26,
            :M27, :M28, :M31, :M32, :M33,
            :M34, :M35, :M36, :M37, :M38, :M39, :M40, :M41
        )
    )");

        qry.bindValue(":MMU", MMU);
        qry.bindValue(":M01", M01);
        qry.bindValue(":M02", M02);
        qry.bindValue(":M03", M03);
        qry.bindValue(":M04", M04);
        qry.bindValue(":M05", M05);
        qry.bindValue(":M06", M06);
        qry.bindValue(":M07", M07);
        qry.bindValue(":M08", M08);
        qry.bindValue(":M09", M09);
        qry.bindValue(":M10", M10);
        qry.bindValue(":M11", M11);
        qry.bindValue(":M12", M12);
        qry.bindValue(":M13", M13);
        qry.bindValue(":M14", M14);
        qry.bindValue(":M15", M15);
        qry.bindValue(":M16", M16);
        qry.bindValue(":M17M18", M17 + " " + M18);
        qry.bindValue(":M19", M19);
        qry.bindValue(":M20", M20);
        qry.bindValue(":M21", M21);
        qry.bindValue(":M22", M22);
        qry.bindValue(":M23", M23);
        qry.bindValue(":M24", M24);
        qry.bindValue(":M25", M25);
        qry.bindValue(":M26", M26);
        qry.bindValue(":M27", M27);
        qry.bindValue(":M28", M28);
        qry.bindValue(":M31", M31);
        qry.bindValue(":M32", M32);
        qry.bindValue(":M33", M33);
        qry.bindValue(":M34", M34);
        qry.bindValue(":M35", M35);
        qry.bindValue(":M36", M36);
        qry.bindValue(":M37", M37);
        qry.bindValue(":M38", M38);
        qry.bindValue(":M39", M39);
        qry.bindValue(":M40", M40);
        qry.bindValue(":M41", M41);

        if (!qry.exec()) {
            QMessageBox::warning(this, "FALHA", "Falha na gravação do cabeçalho - consulta qry\n" + qry.lastError().text());
            return;
        }





        // gravação dos itens - início
        // gravação dos itens - início  - melhorado --------------------------
        int Qtdep = ui->tw_produtos->rowCount();
        bool witem = true; // flag de sucesso

        for (int i = 0; i < Qtdep; ++i) {

            QString nitem = QString::number(i + 1);

            // Ler valores do tw_produtos
            QString codkap = ui->tw_produtos->item(i, 0)->text().left(20);
            QString codnocli = ui->tw_produtos->item(i, 1)->text().left(30);
            QString descri = ui->tw_produtos->item(i, 2)->text().trimmed().left(180);
            if(descri.isEmpty()){
                QMessageBox::information(this,"AVISO","<h3>A Descrição do produto não pode ficar em branco</h3>");
                return;
            }

            // Desconto
            QString descoStr = ui->tw_produtos->item(i, 3)->text().replace(",", ".");
            double desco = descoStr.toDouble();

            // Preço unitário
            QString puStr = ui->tw_produtos->item(i, 4)->text().remove(".").replace(",", ".");
            double preunit = puStr.toDouble();

            QString qtde = ui->tw_produtos->item(i, 5)->text();
            QString prazo = ui->tw_produtos->item(i, 6)->text();

            // IPI
            QString ipiStr = ui->tw_produtos->item(i, 7)->text().replace(",", ".");
            double ipi = ipiStr.toDouble();

            // Valor total
            QString vtStr = ui->tw_produtos->item(i, 8)->text().remove('.').replace(',', '.');
            double valtot = vtStr.toDouble();
            QString ncm = ui->tw_produtos->item(i, 9)->text().left(10);
            QString st = ui->tw_produtos->item(i, 10)->text().replace(',', '.');

            double precoCheio = ui->tw_produtos->item(i, 11)->text().toDouble();
            QString Pimportado = ui->tw_produtos->item(i, 12)->text();
            QString Imagem = ui->tw_produtos->item(i, 13)->text();
            QString PesoL = ui->tw_produtos->item(i, 14)->text().replace(',', '.');

            QString codcli = ui->tw_produtos->item(i, 18)->text().left(20); // codigo no fornecedor
            QString imprime = ui->tw_produtos->item(i, 15)->text();
            QString CodNoConcorrente = ui->tw_produtos->item(i, 16)->text().left(20);
            QString icms = ui->tw_produtos->item(i, 17)->text();
            QString grupo = ui->tw_produtos->item(i, 19)->text().toUpper().left(3); //série

            QSqlQuery qrySp(infokapDB);
            qrySp.prepare("INSERT INTO CotacoesProdutos "
                          "(NDaCotacao, CodigoProduto, nitem, [Descrição], Qtde, PreçoUnitario, IPI, Total, Prazo, CodCli, Imprime, Imagem, NCM, "
                          "PerDesc, CodProdCli, ST, PesoL, Pimportado, PrecoCheio, CodNoConcorrente, ICMS, b1_grupo) "
                          "VALUES "
                          "(:MMU, :codkap, :nitem, :descri, :qtde, :preunit, :ipi, :valtot, :prazo, :codcli, :imprime, :Imagem, :ncm,"
                          " :desco, :codnocli, :st, :PesoL, :Pimportado, :precoCheio, :CodNoConcorrente, :icms, :b1_grupo)");

            qrySp.bindValue(":MMU", MMU);
            qrySp.bindValue(":codkap", codkap);
            qrySp.bindValue(":nitem", nitem);
            qrySp.bindValue(":descri", descri);
            qrySp.bindValue(":qtde", qtde);
            qrySp.bindValue(":preunit", preunit);
            qrySp.bindValue(":ipi", ipi);
            qrySp.bindValue(":valtot", valtot);
            qrySp.bindValue(":prazo", prazo);
            qrySp.bindValue(":codcli", codcli);
            qrySp.bindValue(":imprime", imprime);
            qrySp.bindValue(":Imagem", Imagem);
            qrySp.bindValue(":ncm", ncm);
            qrySp.bindValue(":desco", desco);
            qrySp.bindValue(":codnocli", codnocli);
            qrySp.bindValue(":st", st);
            qrySp.bindValue(":PesoL", PesoL);
            qrySp.bindValue(":Pimportado", Pimportado);
            qrySp.bindValue(":precoCheio", precoCheio);
            qrySp.bindValue(":CodNoConcorrente", CodNoConcorrente);
            qrySp.bindValue(":icms", icms);
            qrySp.bindValue(":b1_grupo", grupo);
            // qDebug() << MMU << codkap << nitem << descri << qtde << preunit << ipi << valtot << prazo <<
            //     codcli << imprime << Imagem << ncm << desco << codnocli << st << PesoL << Pimportado << precoCheio << CodNoConcorrente << icms << grupo;
            if (!qrySp.exec()) {
                QMessageBox::warning(this, "Cotação", "Falha na gravação dos itens (produtos).\n"+qrySp.lastError().text());
                witem = false;
                break; // parar na primeira falha
            }
        }
        // gravação dos itens - fim

        if(cab == "S" && witem ){
            QMessageBox::information(this,"Orçamento","Cadastro feito com sucesso");
            //orcamento_ti::alterouNp=true;

            //verificando se o contato, e-mail, ddd e telefone do cliente foi alterado
            if (M03.isEmpty()){
                QString wcontato  = m_contato_original.simplified().left(15);
                QString wemail = m_email_original.simplified().left(80);
                QString wddd = m_ddd_original.trimmed().rightJustified(3, '0');
                QString wtel = m_telefone_original.simplified().left(15);

                if(wcontato != M15 || wemail != M16 || wddd != M17 || wtel != M18 ){

                    QMessageBox::StandardButton resposta=QMessageBox::question(this,"ATUALIZAR","<h3>Você alterou o cadastro do cliente, deseja atualizar o Protheus?</h3>",
                                                                                 QMessageBox::No|QMessageBox::Yes, QMessageBox::No);

                    if (resposta == QMessageBox::Yes) {
                        QSqlDatabase ProtheusDBa = QSqlDatabase::database("dba");
                        QSqlQuery qryAtualizar(ProtheusDBa);

                        qryAtualizar.prepare(
                            "UPDATE SA1010 SET "
                            "A1_CONTATO = :contato, "
                            "A1_EMLCOM  = :email, "
                            "A1_DDD     = :ddd, "
                            "A1_TEL     = :telefone "
                            "WHERE A1_COD = :cod AND A1_LOJA = :loja"
                            );

                        qryAtualizar.bindValue(":contato", M15);
                        qryAtualizar.bindValue(":email", M16);
                        qryAtualizar.bindValue(":ddd", M17);
                        qryAtualizar.bindValue(":telefone", M18);
                        qryAtualizar.bindValue(":cod", M01);
                        qryAtualizar.bindValue(":loja", M02);

                        if (!qryAtualizar.exec()) {
                            QMessageBox::warning(this, "FALHA", "Falha ao atualizar cadastro no Protheus. - consulta qryAtualizar\n"+qryAtualizar.lastError().text());
                            return;
                        }
                    }
                }
            }
        }

        // Se chegou aqui sem erros, efetiva no banco
        if (infokapDB.commit()) {
            QMessageBox::information(this, "Sucesso", "Orçamento gravado com sucesso!");

            // Aqui você faz a parte opcional do Protheus (SA1010)
            // ...

            this->close();
        } else {
            infokapDB.rollback();
            QMessageBox::critical(this, "Erro", "Falha ao efetivar (commit) os dados.");
        }

    } catch (const std::exception &e) {
        infokapDB.rollback(); // Desfaz qualquer gravação parcial
        QMessageBox::warning(this, "Falha Gravíssima", QString::fromStdString(e.what()));
    }




}

// --- Função auxiliar para validar campos ---
bool orcamento_novo::validarCamposObrigatorios()
{
    // Lista de campos obrigatórios do tipo QLineEdit
    QVector<QPair<QLineEdit*, QString>> camposObrigatorios = {
        {ui->txt_contato_1, "<h3>Você não informou o nome do contato</h3>"},
        {ui->txt_Emailcontato_1, "<h3>Você não informou o e-mail</h3>"},
        {ui->txt_codCli, "<h3>Você não informou o código do cliente</h3>"},
        {ui->txt_loja, "<h3>Você não informou a loja do cliente</h3>"},
        {ui->txt_regiao, "<h3>Você não informou a região do cliente.<br>Se for Prospect, altere o registro do seu cadastro.</h3>"}
    };

    for (const auto &campo : camposObrigatorios) {
        if (campo.first->text().trimmed().isEmpty()) {
            QMessageBox::information(this, "AVISO", campo.second);
            campo.first->setFocus();
            return false;
        }
    }

    // Verificação de QComboBox obrigatório
    if (ui->cb_dll->currentText().isEmpty()) {
        QMessageBox::information(this, "AVISO", "<h3>Você não informou o prazo de pagamento.</h3>");
        ui->cb_dll->setFocus();
        return false;
    }

    // Verificação da quantidade de itens na tabela
    if (ui->tw_produtos->rowCount() < 1) {
        QMessageBox::information(this, "AVISO", "<h3>Você não informou itens para este orçamento</h3>");
        ui->tw_produtos->setFocus();
        return false;
    }

    // Todos os campos obrigatórios foram preenchidos
    return true;
}

void orcamento_novo::on_bt_dadosdo_cliente_clicked()
{
    Sessao_dados_do_cliente s; // Criando uma instância da struct
    s.codigo_cli = ui->txt_codCli->text();
    s.loja_cli = ui->txt_loja->text();
    s.cnpj_cli = ui->txt_cnpjCli->text();

    dados_do_cliente *f_dados_do_cliente = new dados_do_cliente(s, this);
    f_dados_do_cliente->exec();
    delete f_dados_do_cliente;
}






void orcamento_novo::on_bt_abrirOP_clicked()
{

    // Verifica se há uma linha selecionada
    const int linha = ui->tw_produtos->currentRow();
    if (linha < 0) {
        QMessageBox::information(this, "AVISO", "<h3>Informe um produto primeiro.</h3>");
        return;
    }

    // 1. Criar uma janela (Dialog) para mostrar a tabela
    QDialog *janelaDetalhes = new QDialog(this);
    janelaDetalhes->setWindowTitle("Explosão de Necessidades - O que falta?");
    janelaDetalhes->resize(900, 500);

    // 2. Criar o Layout e o QTableView por código
    QVBoxLayout *layout = new QVBoxLayout(janelaDetalhes);
    QTableView *tv_detalhes = new QTableView(janelaDetalhes);
    layout->addWidget(tv_detalhes);

    // 3. Criar o Model (model_estru)
    QStandardItemModel *model_estru = new QStandardItemModel(this);
    model_estru->setHorizontalHeaderLabels({
        "Pai (Montagem)",
        "Componente (Falta)",
        "Descrição do\nComponente",
        "Qtde Necessária",
        "Saldo Livre",
        "Em Processo\n   (OP/OC)",
        "Situação do\nEstoque",
        "Prazo (Dias)"
    });



    // 4. Chamar a função de cálculo (que vai preencher o model_estru)
    QString codp = ui->tw_produtos->item(linha,0)->text();
    int qtde_pedida = ui->tw_produtos->item(linha,5)->text().toInt();

    int estoque_pai = ui->txt_estoquelivre->text().toInt();
    int falta_pai = qtde_pedida - estoque_pai;
    //qDebug() << "codp "+codp << "qtde_pedida "+QString::number(qtde_pedida) << "estoque_pai "+QString::number(estoque_pai) << "falta_pai "+QString::number(falta_pai);
    if (falta_pai > 0) {
        // A função preenche o model_estru recursivamente
       // qDebug() << "No botao " << codp << falta_pai << model_estru;
        calcularPrazoEstrutura(codp, falta_pai, model_estru);

    } else {
        QMessageBox::information(this, "Aviso", "Este produto possui estoque total para pronta entrega.");
        delete janelaDetalhes;
        return;
    }

    // 5. Conectar o Model à View e ajustar visual
    tv_detalhes->setModel(model_estru);
    tv_detalhes->setEditTriggers(QAbstractItemView::NoEditTriggers); // Somente leitura
    tv_detalhes->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // Ajusta colunas
    tv_detalhes->setAlternatingRowColors(true);

    janelaDetalhes->exec(); // Abre a janela

}





int orcamento_novo::sub_tempo_TS()                           // Tempo(dias) p/ montar o produto
{
    QString cod_produto = ui->txt_codP->text().toUpper();
    int qtde = ui->txt_qtde->text().toUInt();
    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
    QSqlQuery qrypz(ProtheusDB);
    qrypz.prepare("SELECT G2_LOTEPAD "
                  "FROM SG2010 "
                  "WHERE G2_PRODUTO = :cod_produto "
                  "AND D_E_L_E_T_ = ' '");
    qrypz.bindValue(":cod_produto",cod_produto);
    if(!qrypz.exec()){
        QMessageBox::warning(this,"ERRO","falha na consulta qrypz ");
        return 0;
    }
    qrypz.first();
    double G2_LOTEPAD = 0;
    double TprodS = 0;
    double TPP_p = 0;        // tempo para fabricar o produto em função da quantidade
    do{
        G2_LOTEPAD = qrypz.value(0).toDouble();
        if(G2_LOTEPAD > 0){
            TprodS += (3600/G2_LOTEPAD);
        }
    }while(qrypz.next());
    if(TprodS!=0){
        TPP_p = int((qtde * TprodS/3600/7)+0.6);    // 7 horas trabalhadas por dia
    };
    return TPP_p;
}

void orcamento_novo::on_bt_correios_clicked()
{
    ui->txt_pesoTotal->selectAll();
    ui->txt_pesoTotal->copy();

    QString vendedor =  m_dados.vendedor.trimmed() ;
    if(vendedor=="000001"){
        QDesktopServices::openUrl(QUrl("L:/InfokapQt/Frete/correio2.exe"));
    }else{
        QDesktopServices::openUrl(QUrl("C:/Correio/correio.exe"));
    }
}


void orcamento_novo::on_cb_frete_currentTextChanged(const QString &arg1)
{
    QString tipo_f = ui->cb_frete->currentText();

    for(int i=0 ; i<ui->tw_produtos->rowCount() ; i++) {

        int prazo_orig = ui->tw_produtos->item(i,6)->text().toInt(); //  prazo
        if(tipo_f == "CIF"){
            ui->tw_produtos->setItem(i, 6, new QTableWidgetItem(QString::number(prazo_orig + 3)));
        }else{
            ui->tw_produtos->setItem(i, 6, new QTableWidgetItem(QString::number(prazo_orig - 3)));
        }
    }
}


void orcamento_novo::on_txt_cod_concorrente_editingFinished()
{
    on_similar();
}

void orcamento_novo::on_similar()
{
    QString concorrente = ui->txt_cod_concorrente->text().toUpper().trimmed();
    if(concorrente.isEmpty()){
        return;
    }
/*
    ui->tw_similaridade->setVisible(true);
    ui->tw_similaridade->resize(600,250);

    QSqlDatabase InfokapDB = QSqlDatabase::database("dbi");
    QSqlQuery qry_simi(InfokapDB);
    qry_simi.prepare("SELECT [Codigo concorrente], [Codigo Kap], Ativo"
                     " FROM CodigoSimilar"
                     " WHERE [Codigo concorrente] LIKE :concorrente"
                     " AND Ativo = 'S'"
                     " GROUP BY [Codigo concorrente], [Codigo Kap], Ativo");
    qry_simi.bindValue(":concorrente","%"+concorrente+"%");
    if(!qry_simi.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta qry_simi\n"+qry_simi.lastError().text());
        return;
    }


    int cLinhas = 0;
    ui->tw_similaridade->setColumnCount(3);
    while (qry_simi.next()) {
        ui->tw_similaridade->insertRow(cLinhas);
        ui->tw_similaridade->setItem(cLinhas,0,new QTableWidgetItem(qry_simi.value(0).toString()));
        ui->tw_similaridade->setItem(cLinhas,1,new QTableWidgetItem(qry_simi.value(1).toString()));
        ui->tw_similaridade->setItem(cLinhas,2,new QTableWidgetItem(qry_simi.value(2).toString()));
        ui->tw_similaridade->setRowHeight(cLinhas,7);   //altura da linha
        cLinhas++;
    };
    ui->tw_similaridade->setColumnWidth(0,280);
    ui->tw_similaridade->setColumnWidth(1,280);
    ui->tw_similaridade->setColumnWidth(2,40);

    QStringList cabecalhos={"Código no concorrente","Código na Kap", "Ativo"};
    ui->tw_similaridade->setHorizontalHeaderLabels(cabecalhos);
    ui->tw_similaridade->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tw_similaridade->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tw_similaridade->verticalHeader()->setVisible(false);
    */
}

void orcamento_novo::on_txt_codP_no_cli_editingFinished()
{
    QString codpCli = ui->txt_codP_no_cli->text().toUpper().trimmed();
    if(codpCli.isEmpty()){
        return;
    }

    int qtdeDeLinhas = ui->tw_produtos->rowCount() ;   //para sempre incluir novo produto no fim da lista
    contLi=qtdeDeLinhas;

    QString loja = ui->txt_loja->text();
    QString codcli = ui->txt_codCli->text();

    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
    QSqlQuery qrycc(ProtheusDB);
    qrycc.prepare("SELECT A7_PRODUTO FROM SA7010"
                  " WHERE A7_CLIENTE = :codcli"
                  " AND A7_LOJA = :loja"
                  " AND A7_CODCLI = :codpCli"
                  " AND D_E_L_E_T_ <>'*'");
    qrycc.bindValue(":codcli",codcli);
    qrycc.bindValue(":loja",loja);
    qrycc.bindValue(":codpCli",codpCli);
    if(!qrycc.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta qrycc\n"+qrycc.lastError().text());
        return;
    }
    qrycc.first();
    QString codp = qrycc.value(0).toString().trimmed();

    if(codp.isEmpty()){
        QMessageBox::information(this,"AVISO","<h3>Não temos um produto correspondente cadastrado</h3>");
        ui->txt_codP_no_cli->clear();
        ui->txt_qtde->clear();
        ui->txt_qtde->setFocus();
        return;
    }
    ui->txt_codP->setText(codp);

    on_txt_codP_editingFinished();
}


void orcamento_novo::on_txt_codCli_editingFinished()
{
    cliloja();
}


void orcamento_novo::on_txt_cnpjCli_editingFinished()
{
    ui->bt_importar_excel->setEnabled(true);

    QString cnpjcli = ui->txt_cnpjCli->text().trimmed();
    if(cnpjcli.isEmpty()){
        return;
    }
    if(cnpjcli.simplified().length() == 18){
        QString aux = cnpjcli;
        aux.remove(QChar('.'),Qt::CaseInsensitive);
        aux.remove(QChar('/'),Qt::CaseInsensitive);
        aux.remove(QChar('-'),Qt::CaseInsensitive);
        cnpjcli=aux;
        ui->txt_cnpjCli->clear();
        ui->txt_cnpjCli->setText(cnpjcli);
    }

    cliloja();
}


void orcamento_novo::on_txt_prospect_editingFinished()
{
    if(ui->txt_prospect->text().isEmpty()){
        return;
    }
    prospect_cliloja();
}

void orcamento_novo::prospect_cliloja()
{
    ui->tw_cliloja->setRowCount(0);

    QString prospect = ui->txt_prospect->text().toUpper().simplified();
    QString codprospect = ui->txt_codprospect->text().simplified();
    QString cnpjprospect = ui->txt_cnpj_prospect->text().simplified();
    QString cpfprospect = ui->txt_cpf_prospect->text().simplified();

    ui->txt_codCli->setText("000001");
    ui->txt_nomeCli->setText("ORCAMENTO");
    ui->txt_prospect->setText(prospect);

    QSqlDatabase infokapDB = QSqlDatabase::database("dbi");
    QSqlQuery qryprospct(infokapDB);

    QString sql = "SELECT * FROM Prospects WHERE ";

    if (!prospect.isEmpty()) {
        qryprospct.prepare(sql + "US_NREDUZ LIKE :val ORDER BY US_NREDUZ");
        qryprospct.bindValue(":val", "%" + prospect + "%");
    } else if (!codprospect.isEmpty()) {
        qryprospct.prepare(sql + "CJ_PROSPE = :val ORDER BY US_NREDUZ");
        qryprospct.bindValue(":val", codprospect);
    } else if (!cpfprospect.isEmpty()) {
        qryprospct.prepare(sql + "CPF = :val");
        qryprospct.bindValue(":val", cpfprospect);
    } else {
        qryprospct.prepare(sql + "US_CGC = :val");
        qryprospct.bindValue(":val", cnpjprospect);
    }

    if (!qryprospct.exec()) {
        QMessageBox::warning(this, "FALHA", "Erro na consulta: " + qryprospct.lastError().text());
        return;
    }

    int row = 0;
    while (qryprospct.next()) {
        ui->tw_cliloja->insertRow(row);

        // Mapeamento direto de colunas do banco para a tabela
        QList<int> colunasBanco = {0, 1, 3, 15, 6, 9, 8, 11, 5};
        for (int i = 0; i < colunasBanco.size(); ++i) {
            ui->tw_cliloja->setItem(row, i, new QTableWidgetItem(qryprospct.value(colunasBanco[i]).toString().trimmed()));
        }

        ui->tw_cliloja->setRowHeight(row, 20); // Aumentado de 7 para 20 (7 é quase invisível)
        row++;
    }

    // Configurações da Tabela
    QStringList cabecalhos = {"Código", "Loja", "Cliente", "CNPJ/CPF", "Endereço", "Bairro", "Cidade", "UF", "Tipo"};
    QList<int> larguras = {50, 25, 120, 110, 150, 120, 120, 35, 40};

    ui->tw_cliloja->setColumnCount(cabecalhos.size());
    ui->tw_cliloja->setHorizontalHeaderLabels(cabecalhos);

    for (int i = 0; i < larguras.size(); ++i) {
        ui->tw_cliloja->setColumnWidth(i, larguras[i]);
    }

    ui->tw_cliloja->setStyleSheet("QTableWidget::item:selected{ background-color: red; }");
    ui->tw_cliloja->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tw_cliloja->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tw_cliloja->verticalHeader()->setVisible(false);

    ui->txt_qtde->setFocus();

    // Preenche os campos de texto com o primeiro resultado (se houver)
    if (qryprospct.first()) {
        ui->txt_cnpjCli->setText(qryprospct.value(15).toString());
        ui->txt_loja->setText(qryprospct.value(1).toString());
        ui->txt_prospect->setText(qryprospct.value(3).toString().trimmed());
        ui->txt_cpf_prospect->setText(qryprospct.value(20).toString());
        ui->txt_codprospect->setText(qryprospct.value(0).toString());
        ui->txt_contato_1->setText(qryprospct.value(4).toString());
        ui->txt_Emailcontato_1->setText(qryprospct.value(14).toString());

        QString dddFormatado = qryprospct.value(12).toString().trimmed().rightJustified(3, '0');
        ui->txt_ddd->setText(dddFormatado);
      //  ui->txt_ddd_original->setText(dddFormatado);

        ui->txt_telefonecontato_1->setText(qryprospct.value(13).toString());
        ui->txt_uf->setText(qryprospct.value(11).toString());
        ui->txt_tipo->setText(qryprospct.value(5).toString());
        ui->txt_celularcontato_1->setText(qryprospct.value(18).toString());
        // ui->txt_p_endereco->setText(qryprospct.value(6).toString());
        // ui->txt_p_bairro->setText(qryprospct.value(9).toString());
        // ui->txt_p_cidade->setText(qryprospct.value(8).toString());
        ui->txt_regiao->setText(qryprospct.value(21).toString().toUpper());
    }
}
void orcamento_novo::on_txt_codprospect_editingFinished()
{
    if(ui->txt_codprospect->text().isEmpty()){
        return;
    }
    prospect_cliloja();
}


void orcamento_novo::on_txt_cnpj_prospect_editingFinished()
{
    QString cnpjcli = ui->txt_cnpj_prospect->text().trimmed();
    if(cnpjcli.isEmpty()){
        return;
    }
    if(cnpjcli.simplified().length() == 18){
        QString aux = cnpjcli;
        aux.remove(QChar('.'),Qt::CaseInsensitive);
        aux.remove(QChar('/'),Qt::CaseInsensitive);
        aux.remove(QChar('-'),Qt::CaseInsensitive);
        cnpjcli=aux;
        ui->txt_cnpj_prospect->clear();
        ui->txt_cnpj_prospect->setText(cnpjcli);
    }
    prospect_cliloja();
}


void orcamento_novo::on_txt_cpf_prospect_editingFinished()
{
    if(ui->txt_cpf_prospect->text().isEmpty()){
        return;
    }
    prospect_cliloja();
}


void orcamento_novo::on_txt_codP_img_semelhante_editingFinished()
{
    int contLiSemelhante = ui->tw_produtos->currentRow();
    QString codpSemelhante = ui->txt_codP_img_semelhante->text().trimmed().toUpper();

    // 1. Proteção: Se não houver linha selecionada,currentRow retorna -1
    if (contLiSemelhante < 0) return;

    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
    QSqlQuery qrypro(ProtheusDB);

    qrypro.prepare("SELECT B1_BITMAP FROM SB1010 "
                   "WHERE B1_COD = :cod AND D_E_L_E_T_ <> '*'");
    qrypro.bindValue(":cod", codpSemelhante);

    if (qrypro.exec()) {
        if (qrypro.next()) { // 2. Verifica se realmente retornou um registro
            QString imagem_prod = qrypro.value(0).toString().trimmed().left(7);

            // 3. Lógica de imagem nula/vazia do Protheus
            if (imagem_prod == "0000000" || imagem_prod.isEmpty()) {
                ui->lb_imagem->clear(); // Limpa o label se não houver imagem
                ui->tw_produtos->setItem(contLiSemelhante, 13, new QTableWidgetItem(""));
            } else {
                QString caminhoCompleto = QString("K:/%1.jpg").arg(imagem_prod);
                QPixmap img(caminhoCompleto);

                if (!img.isNull()) {
                    ui->lb_imagem->setPixmap(img.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                } else {
                    ui->lb_imagem->setText("Erro ao carregar arquivo");
                }

                ui->tw_produtos->setItem(contLiSemelhante, 13, new QTableWidgetItem(imagem_prod));
            }
        } else {
            QMessageBox::information(this, "AVISO", "Código não localizado no banco de dados.");
        }
    } else {
        QMessageBox::critical(this, "ERRO", "Falha técnica na consulta SQL.");
    }
}

