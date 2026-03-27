#include "editar_item.h"
#include "ui_editar_item.h"
#include "orcamento_novo.h"


int e_posicao;

editar_item::editar_item(Sessao_editar_item sessaoE, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::editar_item)
{
    ui->setupUi(this);
    this->dados_item = sessaoE;

    ui->txt_codigoKap->setText(dados_item.codkap_item);
    ui->txt_descricao->setText(dados_item.descricao_item);
    ui->txt_desconto->setText(dados_item.descricao_item);
    ui->txt_precoUnit->setText(dados_item.precoun_ititem);
    ui->txt_qtde->setText(dados_item.qtdedo_item);
    ui->txt_ipi->setText(dados_item.ipi_item);
    ui->txt_ncm->setText(dados_item.ncm_item);
    ui->txt_st->setText(dados_item.st_item);
    ui->txt_desconto->setText(dados_item.desconto_item);

    ui->txt_valtot->setText(dados_item.valortotal_item);

    ui->txt_uf->setText(dados_item.uf_item);
    ui->txt_prazo->setText(dados_item.prazo_item);
    ui->txt_importado->setText(dados_item.import_item);
    ui->txt_peso->setText(dados_item.peso_item);

    ui->txt_codigoNoCli->setText(dados_item.codnocli_item);
    ui->txt_codigoNoForn->setText(dados_item.codnoforn_item);
    ui->txt_codigoNoConcorrente->setText(dados_item.codnoconc_item);

    ui->txt_PL->setText(dados_item.precocheio_item);
    ui->txt_regiao->setText(dados_item.regiao_item);
    ui->txt_icms->setText(dados_item.icms_item);
    ui->txt_cod_imagem->setText(dados_item.cod_imagem_item);

    QLocale brasilLocale(QLocale::Portuguese,QLocale::Brazil);

    QSqlDatabase InfokapDB = QSqlDatabase::database("dbi");
    QSqlQuery qry_rd(InfokapDB);
    qry_rd.prepare("select D1,D2,D3,D4,D5,D6,D7,F1,F2,F3,F4,F5,F6,F7 "
                   "FROM RegraDeDescontoPorUF "
                   "WHERE UF_Desc = :uf");
    qry_rd.bindValue(":uf",dados_item.uf_item);
    if (qry_rd.exec() && qry_rd.next()){

        ui->lb_D1->setText(qry_rd.value(0).toString());
        ui->lb_D2->setText(qry_rd.value(1).toString());
        ui->lb_D3->setText(qry_rd.value(2).toString());
        ui->lb_D4->setText(qry_rd.value(3).toString());
        ui->lb_D5->setText(qry_rd.value(4).toString());
        ui->lb_D6->setText(qry_rd.value(5).toString());
        ui->lb_D7->setText(qry_rd.value(6).toString());

        QString PLa = ui->txt_PL->text().replace(',','.');
        double PL = PLa.toDouble();
        double F1 = qry_rd.value(7).toDouble();
        double F2 = qry_rd.value(8).toDouble();
        double F3 = qry_rd.value(9).toDouble();
        double F4 = qry_rd.value(10).toDouble();
        double F5 = qry_rd.value(11).toDouble();
        double F6 = qry_rd.value(12).toDouble();
        double F7 = qry_rd.value(13).toDouble();
        double D1 =PL*F1;
        double D2 =PL*F2;
        double D3 =PL*F3;
        double D4 =PL*F4;
        double D5 =PL*F5;
        double D6 =PL*F6;
        double D7 =PL*F7;

        ui->bt_D1->setText( brasilLocale.toString(D1,'f',4));
        ui->bt_D2->setText( brasilLocale.toString(D2,'f',4));
        ui->bt_D3->setText( brasilLocale.toString(D3,'f',4));
        ui->bt_D4->setText( brasilLocale.toString(D4,'f',4));
        ui->bt_D5->setText( brasilLocale.toString(D5,'f',4));
        ui->bt_D6->setText( brasilLocale.toString(D6,'f',4));
        ui->bt_D7->setText( brasilLocale.toString(D7,'f',4));
    }
}


editar_item::~editar_item()
{
    delete ui;
}

void editar_item::on_bt_voltar_clicked()
{
    close();
}


void editar_item::on_txt_desconto_editingFinished()
{
    QLocale brasilLocale(QLocale::Portuguese,QLocale::Brazil);
    QString aux0 = ui->txt_desconto->text().replace(',','.');;
    double desconto = aux0.toDouble();

    QString aux1 = ui->txt_PL->text().replace(',','.');
    double preco = aux1.toDouble();

    double preco2 = preco * (1-(desconto/100));
    ui->txt_precoUnit->setText(brasilLocale.toString(preco2,'f',2));

    on_txt_precoUnit_editingFinished();
}

void editar_item::on_txt_precoUnit_editingFinished()
{
    QLocale brasilLocale(QLocale::Portuguese,QLocale::Brazil);
    QString aux2 = ui->txt_precoUnit->text().remove('.').replace(',','.');
    double preco3 = aux2.toDouble();


    int wqtde = ui->txt_qtde->text().toInt();
    double valortotal = 0;
    if(dados_item.regiao_item == "EXT"){
        valortotal =  preco3 * wqtde;
    }else{
        QString aux0 = ui->txt_ipi->text().replace(',','.');
        //double wipi = aux0.toDouble();
        //valortotal =  ((preco3 *((wipi/100)+1))) * wqtde;
        valortotal =  preco3 * wqtde;
    }
    ui->txt_valtot->setText(brasilLocale.toString(valortotal,'f',2));
    double precolista = ui->txt_PL->text().toDouble();
    double desconto = 100-((preco3 * 100) / precolista);
    ui->txt_desconto->setText(brasilLocale.toString(desconto,'f',2));

}

void editar_item::on_txt_qtde_editingFinished()
{
     on_txt_precoUnit_editingFinished();
}


void editar_item::on_txt_ipi_editingFinished()
{
     on_txt_precoUnit_editingFinished();
}


void editar_item::on_bt_salvar_clicked()
{

    // 1. Validação de campo essencial
    if(ui->txt_importado->text().trimmed().isEmpty()){
        QMessageBox::warning(this, "AVISO","Informe se é importado (S/N)");
        return;
    }

    // 2. Atribuição segura (Certifique-se que dados_item existe!)
    try {
        dados_item.descricao_item   = ui->txt_descricao->toPlainText().trimmed();
        dados_item.precoun_ititem   = ui->txt_precoUnit->text();
        dados_item.qtdedo_item      = ui->txt_qtde->text();
        dados_item.desconto_item    = ui->txt_desconto->text();
        dados_item.ipi_item             = ui->txt_ipi->text();
        dados_item.codnocli_item     = ui->txt_codigoNoCli->text().toUpper();
        dados_item.codnoforn_item   = ui->txt_codigoNoForn->text().toUpper();
        dados_item.codnoconc_item   = ui->txt_codigoNoConcorrente->text().toUpper();
        dados_item.prazo_item          = ui->txt_prazo->text();
        dados_item.ncm_item           = ui->txt_ncm->text();
        dados_item.st_item              = ui->txt_st->text();
        dados_item.import_item      = ui->txt_importado->text().toUpper();
        dados_item.peso_item        = ui->txt_peso->text();
        dados_item.valortotal_item  = ui->txt_valtot->text();
        dados_item.icms_item        = ui->txt_icms->text();
        dados_item.regiao_item      = ui->txt_regiao->text().trimmed().toUpper();

        // Tratamento de imagem mais seguro
        QString imgNome = ui->txt_cod_imagem->text().trimmed();
        if(!imgNome.isEmpty()){
            dados_item.cod_imagem_item = imgNome.toLower().endsWith(".jpg") ? imgNome : imgNome + ".jpg";
        }

        // 3. Finalização correta
        this->accept(); // O accept() já encerra o QDialog retornando 1 (Accepted)

    } catch (...) {
        QMessageBox::critical(this, "Erro", "Erro ao processar dados do item.");
    }
}

void editar_item::desconto()
{
    QLocale brasilLocale(QLocale::Portuguese,QLocale::Brazil);

    QString uf = ui->txt_uf->text();

    QSqlDatabase InfokapDB = QSqlDatabase::database("dbi");
    QSqlQuery qry_rd(InfokapDB);
    qry_rd.prepare("select * from RegraDeDescontoPorUF where UF_Desc = :uf");
    qry_rd.bindValue(":uf",uf);
    if (qry_rd.exec() && qry_rd.next()){

        QString aux=ui->txt_PL->text().replace(',','.');
        double PL=aux.toDouble() ;

        QString aux2 = qry_rd.value(e_posicao).toString().replace(',','.');
        double F=aux2.toDouble() ;
        double D = PL*F;
        ui->txt_precoUnit->setText(brasilLocale.toString(D,'f',2));
        on_txt_precoUnit_editingFinished();
    }else{
        QMessageBox::warning(this,"FALHA","Falha na consulta qry_rd\n"+qry_rd.lastError().text());
    }
}

void editar_item::on_bt_D1_clicked()
{
    e_posicao = 10;
    desconto();
}


void editar_item::on_bt_D2_clicked()
{
    e_posicao = 11;
    desconto();
}


void editar_item::on_bt_D3_clicked()
{
    e_posicao = 12;
    desconto();
}


void editar_item::on_bt_D4_clicked()
{
    e_posicao = 13;
    desconto();
}


void editar_item::on_bt_D5_clicked()
{
    e_posicao = 14;
    desconto();
}


void editar_item::on_bt_D6_clicked()
{
    e_posicao = 15;
    desconto();
}


void editar_item::on_bt_D7_clicked()
{
    e_posicao = 16;
    desconto();
}

