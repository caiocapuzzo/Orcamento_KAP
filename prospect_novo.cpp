#include "prospect_novo.h"
#include "ui_prospect_novo.h"

prospect_novo::prospect_novo(SessaoUsuario sessao, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::prospect_novo)
    , m_dados(sessao) // Copia o pacote recebido para a variável m_dados
{
    ui->setupUi(this);

    ui->txt_cnpj->setFocus();

}

prospect_novo::~prospect_novo()
{
    delete ui;
}

void prospect_novo::on_bt_voltar_clicked()
{
    close();
}


void prospect_novo::on_txt_cnpj_editingFinished()
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


void prospect_novo::on_txt_cep_editingFinished()
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


void prospect_novo::on_bt_salvar_clicked()
{
    QSqlDatabase infokapDB = QSqlDatabase::database("dbi");
    QString aux = ui->txt_nome->text().simplified().left(100);
    aux.remove(QChar('/'),Qt::CaseInsensitive);
    QString nome = aux;
    QString fantasia = ui->txt_fantasia->text().simplified().left(30);
    QString loja = "01";
    QString contato = ui->txt_contato->text().simplified().left(50);
    QString tipo = ui->cb_tipo->currentText();

    QString aux_rp1 = ui->txt_endereco->text().simplified().left(50);
    std::replace(aux_rp1.begin(),aux_rp1.end(),QChar::fromLatin1(char(39)),QChar::fromLatin1(char(46)));
    QString endereco = aux_rp1;



    QString aux_rp2 = ui->txt_bairro->text().simplified().left(50);
    std::replace(aux_rp2.begin(),aux_rp2.end(),QChar::fromLatin1(char(39)),QChar::fromLatin1(char(46)));
    QString bairro = aux_rp2;

    QString aux_rp3 = ui->txt_cidade->text().simplified().left(50);
    std::replace(aux_rp3.begin(),aux_rp3.end(),QChar::fromLatin1(char(39)),QChar::fromLatin1(char(46)));
    QString cidade = aux_rp3;

    //    QString endereco = ui->txt_endereco->text().simplified().left(50);
    //    QString Comple_Pro = "";
    //    QString bairro = ui->txt_bairro->text().simplified().left(50);
    //    QString cidade = ui->txt_cidade->text().simplified().left(50);
    QString cep = ui->txt_cep->text();
    QString uf = ui->txt_uf->text().toUpper();
    QString ddd = ui->txt_ddd->text();
    QString telefone = ui->txt_telefone->text();
    QString email = ui->txt_email->text().toLower().left(50);
    QString cnpj = ui->txt_cnpj->text();
    QString cpf = ui->txt_cpf->text();
    QString site = ui->txt_site->text().toLower();
    QString data = QDate::currentDate().toString("dd/MM/yyyy");
    QString celularn = ui->txt_celular->text();
    QString regiao = ui->txt_regiao->text().toUpper();
    QString Comple_Pro = ui->txt_complemento->text().trimmed();


    if(cpf.isEmpty() && cnpj.isEmpty()){
        QMessageBox::information(this,"AVISO","Você não informou o CNPJ ou o CPF");
        return;
    }

    if(uf.size() < 2){
        QMessageBox::information(this,"AVISO","Você não informou o estado (UF)");
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


    QSqlQuery qrySprct(infokapDB);

    qrySprct.prepare("SELECT MAX(CAST(CJ_PROSPE AS INT)) FROM Prospects");
    if (!qrySprct.exec() || !qrySprct.next()) {
        QMessageBox::warning(this, "FALHA", "Erro ao buscar último ID");
        return;
    }
    int ultimo = qrySprct.value(0).toInt() + 1;

    qrySprct.prepare("INSERT INTO Prospects (CJ_PROSPE, CJ_LOJPRO, US_NOME, US_NREDUZ, Contato, US_TIPO, "
                     "US_END, Comple_Pro, US_MUN, US_BAIRRO, US_CEP, US_EST, US_DDD, US_TEL, US_EMAIL, "
                     "US_CGC, Site, Data, Celular, CPF, Regiao) "
                     "VALUES (:id, :loja, :nome, :fantasia, :contato, :tipo, :end, :comple, :mun, :bairro, "
                     ":cep, :est, :ddd, :tel, :email, :cnpj, :site, :data, :cel, :cpf, :regiao)");

    qrySprct.bindValue(":id", QString::number(ultimo));
    qrySprct.bindValue(":loja", loja);
    qrySprct.bindValue(":nome", nome);
    qrySprct.bindValue(":fantasia", fantasia);
    qrySprct.bindValue(":contato", contato);
    qrySprct.bindValue(":tipo", tipo);
    qrySprct.bindValue(":end", endereco);
    qrySprct.bindValue(":comple", Comple_Pro);
    qrySprct.bindValue(":mun", cidade);
    qrySprct.bindValue(":bairro", bairro);
    qrySprct.bindValue(":cep", cep);
    qrySprct.bindValue(":est", uf);
    qrySprct.bindValue(":ddd", ddd);
    qrySprct.bindValue(":tel", telefone);
    qrySprct.bindValue(":email", email);
    qrySprct.bindValue(":cnpj", cnpj);
    qrySprct.bindValue(":site", site);
    qrySprct.bindValue(":data", data);
    qrySprct.bindValue(":cel", celularn);
    qrySprct.bindValue(":cpf", cpf);
    qrySprct.bindValue(":regiao", regiao);

    if(qrySprct.exec()){
        this->close();
    } else {
        QMessageBox::warning(this, "FALHA", "Erro ao inserir: " + qrySprct.lastError().text());
    }

}


void prospect_novo::on_bt_correio_2_clicked()
{
    QString link="https://buscacepinter.correios.com.br/app/endereco/index.php";
    QDesktopServices::openUrl(QUrl(link));
}


void prospect_novo::on_txt_nome_editingFinished()
{
    QString text=ui->txt_nome->text();
    QString with = "ÄÅÁÂÀÃäáâàãÉÊËÈéêëèÍÎÏÌíîïìÖÓÔÒÕöóôòõÜÚÛüúûùÇç";
    QString withOut = "AAAAAAaaaaaEEEEeeeeIIIIiiiiOOOOOoooooUUUuuuuCc";
    for (int i = 0; i < with.size(); i++)
    {
        text = text.replace( with[i], withOut[i] );
    }
    ui->txt_nome->setText(text);
    ui->txt_fantasia->setFocus();
}


void prospect_novo::on_txt_fantasia_editingFinished()
{
    QString text=ui->txt_fantasia->text();
    QString with = "ÄÅÁÂÀÃäáâàãÉÊËÈéêëèÍÎÏÌíîïìÖÓÔÒÕöóôòõÜÚÛüúûùÇç";
    QString withOut = "AAAAAAaaaaaEEEEeeeeIIIIiiiiOOOOOoooooUUUuuuuCc";
    for (int i = 0; i < with.size(); i++)
    {
        text = text.replace( with[i], withOut[i] );
    }
    ui->txt_fantasia->setText(text);
    ui->txt_contato->setFocus();
}


void prospect_novo::on_txt_cpf_editingFinished()
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

