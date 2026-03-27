#include "prospect_editar.h"
#include "ui_prospect_editar.h"

prospect_editar::prospect_editar(SessaoUsuario sessao, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::prospect_editar)
    , m_dados(sessao) // Copia o pacote recebido para a variável m_dados
{
    ui->setupUi(this);

    QSqlDatabase infokapDB = QSqlDatabase::database("dbi");
    int id = sessao.id;

    QSqlQuery query(infokapDB);
    query.prepare("Select * from Prospects where CJ_PROSPE = :id");
    query.bindValue(":id",QString::number(id));
    if(!query.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta query\n"+query.lastError().text());
    }
    if(query.next()){
        ui->txt_numero->setText(query.value(0).toString());
        ui->txt_uf->setText(query.value(11).toString());
        ui->txt_cep->setText(query.value(10).toString());
        ui->txt_ddd->setText(query.value(12).toString());
        ui->txt_cnpj->setText(query.value(15).toString());
        ui->txt_cpf->setText(query.value(20).toString());
        ui->txt_nome->setText(query.value(2).toString());
        ui->txt_site->setText(query.value(16).toString());
        ui->txt_email->setText(query.value(14).toString());
        ui->txt_bairro->setText(query.value(9).toString());
        ui->txt_cidade->setText(query.value(8).toString());
        ui->txt_celular->setText(query.value(18).toString());
        ui->txt_contato->setText(query.value(4).toString());
        ui->txt_endereco->setText(query.value(6).toString());
        ui->txt_fantasia->setText(query.value(3).toString());
        ui->txt_telefone->setText(query.value(13).toString());
        ui->txt_regiao->setText(query.value(21).toString());
        ui->cb_tipo->setCurrentText(query.value(5).toString());
        ui->txt_complemento->setText(query.value(7).toString());
    }

}

prospect_editar::~prospect_editar()
{
    delete ui;
}

void prospect_editar::on_bt_voltar_clicked()
{
    close();
}


void prospect_editar::on_bt_salvar_clicked()
{
    int id = ui->txt_numero->text().toInt();
    QString loja = "01";
    QString nome = ui->txt_nome->text().simplified();
    QString fantasia = ui->txt_fantasia->text().simplified();
    QString contato = ui->txt_contato->text().simplified();
    QString tipo = ui->cb_tipo->currentText();
    QString endereco = ui->txt_endereco->text().simplified();
    QString cidade = ui->txt_cidade->text().simplified();
    QString bairro = ui->txt_bairro->text().simplified();
    QString cep = ui->txt_cep->text();
    QString uf = ui->txt_uf->text().toUpper();
    QString ddd = ui->txt_ddd->text();
    QString telefone = ui->txt_telefone->text();
    QString email = ui->txt_email->text().toLower();
    QString cnpj = ui->txt_cnpj->text();
    QString cpf = ui->txt_cpf->text();
    QString site = ui->txt_site->text().toLower();
    QString celular = ui->txt_celular->text();
    QString regiao = ui->txt_regiao->text().toUpper();
    QString Comple_Pro = ui->txt_complemento->text().trimmed();

    if(cpf.isEmpty() && cnpj.isEmpty()){
        QMessageBox::information(this,"AVISO","Você não informou o CNPJ ou o CPF");
        return;
    }

    if(fantasia.isEmpty()){
        QMessageBox::information(this,"AVISO","Você não informou o nome fantasia");
        return;
    }
    if(nome.isEmpty()){
        QMessageBox::information(this,"AVISO","Você não informou o nome do cliente");
        return;
    }
    if(contato.isEmpty()){
        QMessageBox::information(this,"AVISO","Você não informou o nome da pessoa de contato");
        return;
    }
    if(email.isEmpty()){
        QMessageBox::information(this,"AVISO","Você não informou o e-mail");
        return;
    }

    if(tipo.isEmpty()){
        QMessageBox::information(this,"AVISO","Você não informou o tipo de cliente");
        return;
    }

    if(regiao.isEmpty()){
        QMessageBox::information(this,"AVISO","Você não informou região");
        return;
    }

    QSqlDatabase infokapDB = QSqlDatabase::database("dbi");
    QSqlQuery query(infokapDB);
    query.prepare(
        "UPDATE Prospects SET "
        "CJ_PROSPE = :id, "
        "CJ_LOJPRO = :loja, "
        "US_NOME = :nome, "
        "US_NREDUZ = :fantasia, "
        "Contato = :contato, "
        "US_TIPO = :tipo, "
        "US_END = :endereco, "
        "US_MUN = :cidade, "
        "US_BAIRRO = :bairro, "
        "US_CEP = :cep, "
        "US_EST = :uf, "
        "US_DDD = :ddd, "
        "US_TEL = :telefone, "
        "US_EMAIL = :email, "
        "US_CGC = :cnpj, "
        "Site = :site, "
        "Celular = :celular, "
        "cpf = :cpf, "
        "Comple_Pro = :comple_pro, "
        "Regiao = :regiao "
        "WHERE CJ_PROSPE = :id"
        );

    query.bindValue(":id", id);
    query.bindValue(":loja", loja);
    query.bindValue(":nome", nome);
    query.bindValue(":fantasia", fantasia);
    query.bindValue(":contato", contato);
    query.bindValue(":tipo", tipo);
    query.bindValue(":endereco", endereco);
    query.bindValue(":cidade", cidade);
    query.bindValue(":bairro", bairro);
    query.bindValue(":cep", cep);
    query.bindValue(":uf", uf);
    query.bindValue(":ddd", ddd);
    query.bindValue(":telefone", telefone);
    query.bindValue(":email", email);
    query.bindValue(":cnpj", cnpj);
    query.bindValue(":site", site);
    query.bindValue(":celular", celular);
    query.bindValue(":cpf", cpf);
    query.bindValue(":comple_pro", Comple_Pro);
    query.bindValue(":regiao", regiao);
    if(query.exec()){
        QMessageBox::information(this,"Alterar Prospect","Alteração feita com sucesso");
        this->close();
    }else{
        QMessageBox::information(this,"Alterar Prospect","Falha na Alteração");
    }
}


void prospect_editar::on_bt_correio_2_clicked()
{
    QString link="https://buscacepinter.correios.com.br/app/endereco/index.php";
    QDesktopServices::openUrl(QUrl(link));
}


void prospect_editar::on_txt_cep_editingFinished()
{
    QSqlDatabase infokapDB = QSqlDatabase::database("dbi");
    QSqlQuery qrycep (infokapDB);
    QString wcep = ui->txt_cep->text();

    qrycep.prepare("SELECT * FROM CEP"
                   " WHERE CEP = :wcep");
    qrycep.bindValue(":wcep",wcep);
    if(!qrycep.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta qrycep\n"+qrycep.lastError().text());
        return;
    }

    if(qrycep.next()){
        QString tipo = qrycep.value(1).toString().trimmed();
        QString endereco = qrycep.value(2).toString().trimmed();
        ui->txt_endereco->setText(tipo + " " + endereco);
        ui->txt_bairro->setText(qrycep.value(3).toString().trimmed());
        ui->txt_cidade->setText(qrycep.value(4).toString().trimmed());
        ui->txt_uf->setText(qrycep.value(5).toString().trimmed());
        ui->txt_ddd->setText(qrycep.value(6).toString().trimmed());
        ui->txt_endereco->setFocus();
    }
}


void prospect_editar::on_txt_cnpj_editingFinished()
{
    QString cnpj = ui->txt_cnpj->text().trimmed();
    if(cnpj.isEmpty()){
        ui->txt_endereco->setFocus();
        return;
    }

    if(cnpj.simplified().length() ==18){
        QString aux=cnpj;
        aux.remove(QChar('.'),Qt::CaseInsensitive);
        aux.remove(QChar('/'),Qt::CaseInsensitive);
        aux.remove(QChar('-'),Qt::CaseInsensitive);
        cnpj=aux;
    }

    QSqlDatabase ProtheusDB = QSqlDatabase::database("dbp");
    QSqlQuery qry_ja_cli (ProtheusDB);
    qry_ja_cli.prepare("SELECT A1_CGC"
                       " FROM SA1010"
                       " WHERE A1_CGC = :cnpj"
                       " AND D_E_L_E_T_ =' '" );
    qry_ja_cli.bindValue(":cnpj",cnpj);
    if(!qry_ja_cli.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta qry_ja_cli\n"+qry_ja_cli.lastError().text());
    }
    int ccli=0;
    while(qry_ja_cli.next()){
        ccli++;
    }

    if(ccli>0){
        QMessageBox::information(this,"AVISO", "Este CNPJ já é cliente");
        return;
    }

    QSqlDatabase infokapDB = QSqlDatabase::database("dbi");
    QSqlQuery qry_ja_pro (infokapDB);
    qry_ja_pro.prepare("SELECT US_CGC"
                       " FROM Prospects"
                       " WHERE US_CGC = :cnpj");
    qry_ja_pro.bindValue(":cnpj",cnpj);
    if(!qry_ja_pro.exec()){
        QMessageBox::warning(this,"FALHA","Falha na consulta qry_ja_pro\n"+qry_ja_pro.lastError().text());
        return;
    }
    ccli=0;
    while(qry_ja_pro.next()){
        ccli++;
    }

    if(ccli>0){
        QMessageBox::information(this,"AVISO", "Este CNPJ já é um prospect");
        return;
    }



    QString pesos_A = "543298765432";
    QString pesos_B = "6543298765432";

    if (cnpj == ("00000000000000") || cnpj == ("11111111111111") ||
        cnpj == ("22222222222222") || cnpj == ("33333333333333") ||
        cnpj == ("44444444444444") || cnpj == ("55555555555555") ||
        cnpj == ("66666666666666") || cnpj == ("77777777777777") ||
        cnpj == ("88888888888888") || cnpj == ("99999999999999") ||
        cnpj.length()!=14)
        ui->txt_cnpj->clear();

    //cálculo do primeiro dígito verificador
    int x1a = 0;
    int x1b = 0;
    int x1c = 0;
    int sx1 = 0;

    QString pdv, sdv;

    for(int a = 0; a < 12; a++){
        x1a = cnpj.mid(a,1).toInt();
        x1b = pesos_A.mid(a,1).toInt();
        x1c = x1a * x1b;
        sx1 = sx1 + x1c;
    }
    int dv1  = (sx1 % 11);
    if (dv1 < 2){
        pdv = "0";                        // pdv = primeiro dígito verificador
    }else{
        pdv = QString::number(11-dv1);
    }

    //cálculo do segundo dígito verificador
    int x2a = 0;
    int x2b = 0;
    int x2c = 0;
    int sx2 = 0;

    for(int a = 0; a < 13; a++){
        if(a == 12){
            x2a = pdv.toInt();
        }else{
            x2a = cnpj.mid(a,1).toInt();
        }
        x2b = pesos_B.mid(a,1).toInt();
        x2c = x2a * x2b;
        sx2 = sx2 + x2c;
    }
    int dv2  = (sx2 % 11);
    if (dv2 < 2){
        sdv = "0";                        // sdv = segundo dígito verificador
    }else{
        sdv = QString::number(11 - dv2);
    }

    QString p13 = cnpj.mid(12,1);
    QString p14 = cnpj.mid(13,1);

    QString validar;
    if (p13==pdv && p14==sdv)
        validar = "1";


    if(validar!="1"){
        QMessageBox::warning(this, "ERRO", "CNPJ inválido");
        ui->txt_cnpj->clear();
        ui->txt_cnpj->setFocus();
    }
    ui->txt_nome->setFocus();
}


void prospect_editar::on_txt_cpf_editingFinished()
{
    QString aux=ui->txt_cpf->text().trimmed();
    aux.remove(QChar('.'),Qt::CaseInsensitive);
    aux.remove(QChar('/'),Qt::CaseInsensitive);
    aux.remove(QChar('-'),Qt::CaseInsensitive);
    aux.remove(QChar('_'),Qt::CaseInsensitive);
    ui->txt_cpf->setText(aux);

    //Validando o CPF
    QString cpf = aux;
    QString pesos_A = "098765432";
    QString pesos_B = "0098765432";

    if (cpf == ("00000000000") || cpf == ("11111111111") ||
        cpf == ("22222222222") || cpf == ("33333333333") ||
        cpf == ("44444444444") || cpf == ("55555555555") ||
        cpf == ("66666666666") || cpf == ("77777777777") ||
        cpf == ("88888888888") || cpf == ("99999999999") ||
        cpf.length()!=11)
        ui->txt_cpf->clear();

    //cálculo do primeiro dígito verificador do cpf
    int x1a = 0;
    int x1b = 0;
    int x1c = 0;
    int sx1 = 0;

    QString pdv, sdv;

    for(int a = 0; a < 9; a++){
        if(a == 0){
            x1b = 10;
        }else{
            x1b = pesos_A.mid(a,1).toInt();
        }
        x1a = cpf.mid(a,1).toInt();
        x1c = x1a * x1b;
        sx1 = sx1 + x1c;
    }

    int dv1  = (sx1 % 11);
    if (dv1 < 2){
        pdv = "0";                        // pdv = primeiro dígito verificador
    }else{
        pdv = QString::number(11 - dv1);
    }

    //cálculo do segundo dígito verificador do cpf
    int x2a = 0;
    int x2b = 0;
    int x2c = 0;
    int sx2 = 0;

    for(int a = 0; a < 10; a++){
        if( a == 0){
            x2b = 11;
            x2a = cpf.mid(a,1).toInt();
        }else if(a == 1){
            x2b = 10;
            x2a = cpf.mid(a,1).toInt();
        }else{
            x2b = pesos_B.mid(a,1).toInt();
        }
        x2a = cpf.mid(a,1).toInt();
        x2c = x2a * x2b;
        sx2 = sx2 + x2c;

    }


    int dv2  = (sx2 % 11);
    if (dv2 < 2){
        sdv = "0";                        // sdv = segundo dígito verificador
    }else{
        sdv = QString::number(11 - dv2);
    }

    QString p09 = cpf.mid(9,1);
    QString p10 = cpf.mid(10,1);

    QString validar;
    if (p09 == pdv && p10 == sdv)
        validar = "1";


    if(validar != "1"){
        QMessageBox::warning(this, "ERRO", "CPF inválido");
        ui->txt_cpf->clear();
        //ui->txt_cpf->setFocus();
    }
    ui->txt_nome->setFocus();

}

