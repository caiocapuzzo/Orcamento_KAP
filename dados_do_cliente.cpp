#include "dados_do_cliente.h"
#include "ui_dados_do_cliente.h"




dados_do_cliente::dados_do_cliente(Sessao_dados_do_cliente sessaoC, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dados_do_cliente)
{
    ui->setupUi(this);
     this->dados_cliente = sessaoC;


     // --- CONSULTA 1: Atividades ---
     QSqlDatabase infokapDB = QSqlDatabase::database("dbi");
     QSqlQuery qryativ(infokapDB);

     // Usamos :cliente e :loja como placeholders
     qryativ.prepare("SELECT * FROM ClienteAtividade "
                     "WHERE Cliente = :cliente "
                     "AND LojaCli = :loja");

     qryativ.bindValue(":cliente", dados_cliente.codigo_cli);
     qryativ.bindValue(":loja", dados_cliente.loja_cli);

     if(qryativ.exec() && qryativ.first()) {
         ui->txt_atividades->setText(qryativ.value(3).toString());
         ui->txt_multinacional->setText(qryativ.value(6).toString());
     }

     // --- CONSULTA 2: Protheus ---
     QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
     QSqlQuery qrycli(ProtheusDB);

     qrycli.prepare("SELECT A1_COD, A1_LOJA, A1_NREDUZ, A1_TIPO, A1_CGC, A1_END, "
                    "A1_BAIRRO, A1_MUN, A1_EST, A1_DDD, A1_TEL, A1_CONTATO, "
                    "A1_ULTCOM, A1_DTNASC, A1_EMLCOM, A1_VEND, A1_OBSBLQ, A1_SITUACA "
                    "FROM SA1010 "
                    "WHERE A1_CGC = :cnpj "
                    "AND D_E_L_E_T_ = ' '");

     qrycli.bindValue(":cnpj", dados_cliente.cnpj_cli);

     if(qrycli.exec() && qrycli.first()) {
         QDate dataUltVenda = QDate::fromString(qrycli.value(12).toString(), "yyyyMMdd");
         QDate dataDesde    = QDate::fromString(qrycli.value(13).toString(), "yyyyMMdd");

         ui->txt_ultimavenda->setText(dataUltVenda.toString("dd/MM/yyyy"));
         ui->txt_clientedesde->setText(dataDesde.toString("dd/MM/yyyy"));
         QString obs_cli = deWindowsParaUtf8(qrycli.value(16).toString().simplified());
         //QString obs_cli = QString::fromLocal8Bit(qrycli.value(16).toString().simplified());
         ui->txt_observacoes->setText(obs_cli);
         ui->txt_situacao->setText(qrycli.value(17).toString());
     }


     QSqlQuery qry_ta(ProtheusDB);
     // 1. SQL limpo com placeholders :cliente e :loja
     qry_ta.prepare("SELECT E1_NUM, E1_PARCELA, E1_EMISSAO, E1_VENCTO, E1_VALOR "
                    "FROM SE1010 "
                    "WHERE E1_CLIENTE = :cliente "
                    "AND E1_LOJA = :loja "
                    "AND E1_BAIXA = ' ' "
                    "AND D_E_L_E_T_ = ' '");

     qry_ta.bindValue(":cliente", dados_cliente.codigo_cli);
     qry_ta.bindValue(":loja", dados_cliente.loja_cli);

     if (!qry_ta.exec()) {
         QMessageBox::warning(this,"FALHA","Falha na consulta qry_ta\n"+qry_ta.lastError().text());
         return;
     }

     // 2. Configurações da Tabela (fazemos fora do loop para ganhar performance)



     ui->tw_titulos_abertos->setRowCount(0); // Limpa a tabela antes de começar
     ui->tw_titulos_abertos->setColumnCount(5);
     QStringList cabecalho_b = {"Nº", "Par.", "Emissão", "Vencimento", "Valor (R$)"};
     ui->tw_titulos_abertos->setHorizontalHeaderLabels(cabecalho_b);

     int cont = 0;
     QLocale brasilLocale(QLocale::Portuguese, QLocale::Brazil);

     while (qry_ta.next()) {
         ui->tw_titulos_abertos->insertRow(cont);

         // Coluna 0 e 1: Número e Parcela
         ui->tw_titulos_abertos->setItem(cont, 0, new QTableWidgetItem(qry_ta.value("E1_NUM").toString().trimmed()));
         ui->tw_titulos_abertos->setItem(cont, 1, new QTableWidgetItem(qry_ta.value("E1_PARCELA").toString().trimmed()));

         // Coluna 2 e 3: Datas (Formatando no Qt em vez de no SQL)
         QDate emissao = QDate::fromString(qry_ta.value("E1_EMISSAO").toString(), "yyyyMMdd");
         QDate vencto  = QDate::fromString(qry_ta.value("E1_VENCTO").toString(), "yyyyMMdd");

         ui->tw_titulos_abertos->setItem(cont, 2, new QTableWidgetItem(emissao.toString("dd/MM/yyyy")));
         ui->tw_titulos_abertos->setItem(cont, 3, new QTableWidgetItem(vencto.toString("dd/MM/yyyy")));

         // Coluna 4: Valor formatado
         double valort = qry_ta.value("E1_VALOR").toDouble();
         QTableWidgetItem *valort_a = new QTableWidgetItem(brasilLocale.toString(valort, 'f', 2));
         valort_a->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter); // Alinhado à direita
         ui->tw_titulos_abertos->setItem(cont, 4, valort_a);

         ui->tw_titulos_abertos->setRowHeight(cont, 20); // 10 é muito baixo, 20-25 fica melhor para ler
         cont++;
     }

     // 3. Ajustes visuais finais
     ui->tw_titulos_abertos->setColumnWidth(0,55);
     ui->tw_titulos_abertos->setColumnWidth(1,30);
     ui->tw_titulos_abertos->setColumnWidth(2,70);
     ui->tw_titulos_abertos->setColumnWidth(3,80);
     ui->tw_titulos_abertos->setColumnWidth(4,80);





     // 1. Definição da data de corte (6 meses atrás)
     QDate data_6m = QDate::currentDate().addMonths(-6);

     QSqlQuery qry_tb(ProtheusDB);
     // Usando placeholders :cliente, :loja e :data_corte
     qry_tb.prepare("SELECT E1_NUM, E1_PARCELA, E1_EMISSAO, E1_VENCTO, E1_VALOR, E1_BAIXA "
                    "FROM SE1010 "
                    "WHERE E1_CLIENTE = :cliente "
                    "AND E1_LOJA = :loja "
                    "AND E1_BAIXA > '00000000' " // No Protheus, data de baixa vazia costuma ser ' ' ou '00000000'
                    "AND D_E_L_E_T_ = ' ' "
                    "AND E1_EMISSAO >= :data_corte");

     qry_tb.bindValue(":cliente", dados_cliente.codigo_cli);
     qry_tb.bindValue(":loja", dados_cliente.loja_cli);
     qry_tb.bindValue(":data_corte", data_6m.toString("yyyyMMdd"));

     if (!qry_tb.exec()) {
         QMessageBox::warning(this,"FALHA","Falha na consulta qry_tb\n"+qry_tb.lastError().text());
         return;
     }

     // 2. Configuração inicial do TableWidget
     ui->tw_titulos_baixados->setRowCount(0);
     ui->tw_titulos_baixados->setColumnCount(6);
     QStringList cabecalho = {"Nº", "Par.", "Emissão", "Vencimento", "Valor (R$)", "Baixa em"};
     ui->tw_titulos_baixados->setHorizontalHeaderLabels(cabecalho);

      cont = 0;


     // 3. Loop de preenchimento
     while (qry_tb.next()) {
         ui->tw_titulos_baixados->insertRow(cont);

         // Campos de texto simples
         ui->tw_titulos_baixados->setItem(cont, 0, new QTableWidgetItem(qry_tb.value("E1_NUM").toString().trimmed()));
         ui->tw_titulos_baixados->setItem(cont, 1, new QTableWidgetItem(qry_tb.value("E1_PARCELA").toString().trimmed()));

         // Datas: Emissão, Vencimento e Baixa (Formatando no Qt)
         QDate dEmissao = QDate::fromString(qry_tb.value("E1_EMISSAO").toString(), "yyyyMMdd");
         QDate dVencto  = QDate::fromString(qry_ta.value("E1_VENCTO").toString(), "yyyyMMdd"); // Corrigido para qry_tb
         QDate dBaixa   = QDate::fromString(qry_tb.value("E1_BAIXA").toString(), "yyyyMMdd");

         ui->tw_titulos_baixados->setItem(cont, 2, new QTableWidgetItem(dEmissao.toString("dd/MM/yyyy")));
         ui->tw_titulos_baixados->setItem(cont, 3, new QTableWidgetItem(dVencto.toString("dd/MM/yyyy")));
         ui->tw_titulos_baixados->setItem(cont, 5, new QTableWidgetItem(dBaixa.toString("dd/MM/yyyy")));

         // Valor financeiro
         double valort = qry_tb.value("E1_VALOR").toDouble();
         QTableWidgetItem *itemValor = new QTableWidgetItem(brasilLocale.toString(valort, 'f', 2));
         itemValor->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
         ui->tw_titulos_baixados->setItem(cont, 4, itemValor);

         ui->tw_titulos_baixados->setRowHeight(cont, 20);
         cont++;
     }

     // 4. Ajustes de largura de coluna (Opcional: usar Stretch)
     ui->tw_titulos_baixados->setColumnWidth(0, 55);
     ui->tw_titulos_baixados->setColumnWidth(1, 35);
     ui->tw_titulos_baixados->setColumnWidth(2,70);
     ui->tw_titulos_baixados->setColumnWidth(3,80);
     ui->tw_titulos_baixados->setColumnWidth(4,80);
     ui->tw_titulos_baixados->setColumnWidth(5,80);

   //  ui->tw_titulos_baixados->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // Deixa o Qt ajustar o resto
}


QString dados_do_cliente::deWindowsParaUtf8(const QVariant &v) {
    QByteArray ba = v.toByteArray();

    // Tenta converter de Windows-1252 para a string Unicode do Qt
    //return QString::fromEncodedData(ba, "Windows-1252");

    return QString::fromLatin1(ba);
   // return QString::fromLocal8Bit(ba);
}

dados_do_cliente::~dados_do_cliente()
{
    delete ui;
}

void dados_do_cliente::on_bt_voltar_clicked()
{
    close();
}

