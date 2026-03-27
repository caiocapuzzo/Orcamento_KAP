#ifndef STRUCT_COMUM_H
#define STRUCT_COMUM_H

#include <QString>
#include <QDate>

struct   SessaoUsuario {
    QString usuario;
    QString sigla;
    QString regiao;
    QString vendedor;
    QString uf;
    int id;

};

struct Sessao_dados_do_cliente {
    QString codigo_cli;
    QString loja_cli;
    QString cnpj_cli;

};


struct Sessao_editar_item {
    QString codkap_item;
    QString descricao_item;
    QString desconto_item;
    QString precoun_ititem;
    QString qtdedo_item;
    QString prazo_item;
    QString ipi_item;
    QString valortotal_item;
    QString ncm_item;
    QString st_item;
    QString precocheio_item;
    QString icms_item;
    QString codnocli_item;
    QString codnoforn_item;
    QString codnoconc_item;
    QString grupo_item;
    QString uf_item;
    QString import_item;
    QString peso_item;
    QString regiao_item;
    QString cod_imagem_item;
};


#endif // STRUCT_COMUM_H
