# Controle de Tags

Sistema em C, executado no terminal, para controle de tags veiculares e dispositivos de acesso de uma empresa de portaria remota.

## Funcionalidades

- Login de operador com até três tentativas
- Cadastro de tags, com data preenchida automaticamente
- Listagem das tags por mês de cadastro
- Busca por código
- Alteração de status: ativa, cancelada, perdida ou substituída
- Registro da cobrança (tags de substituição não são cobradas)
- Dados salvos em arquivo entre uma execução e outra

## Como executar

```bash
gcc -Wall tags.c -o controle_tags
./controle_tags
```

Na primeira execução, escolha **2. Cadastrar** para criar o usuário.

Os dados ficam nos arquivos `tags.dat` e `usuario.dat`, criados na pasta de execução. Eles não são enviados ao repositório, porque contêm dados pessoais.

## Desenvolvimento

Projeto desenvolvido por mim, com o apoio do Claude (Anthropic) como ferramenta de correção de erros e direcionamento na estruturação do código.
