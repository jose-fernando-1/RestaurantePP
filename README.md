# RestaurantePP

# Simulação de Restaurante com Threads

## Descrição do Projeto

Este projeto simula a interação de **clientes** e **garçons** em um restaurante, onde múltiplos clientes tentam ocupar mesas disponíveis e são atendidos por garçons. A implementação usa **threads** em C e mecanismos de sincronização como **mutexes** e **variáveis condicionais** para garantir que o acesso às mesas seja seguro e ordenado.

## Estrutura do Código

### Mesas
- Cada mesa é representada por uma estrutura que contém um **mutex** (`lock`) para sincronização e uma **condicional** (`cond`) para indicar quando a mesa está ocupada.
- As mesas são compartilhadas entre clientes e garçons.

### Garçons
- Os garçons são representados por threads que atendem mesas específicas. Cada garçom é associado a uma mesa e aguarda até que um cliente ocupe a mesa para iniciar o atendimento.

### Clientes
- Clientes são representados por threads que tentam encontrar uma mesa livre. Se uma mesa estiver disponível, o cliente a ocupa e sinaliza o garçom.

## Parâmetros de Concorrência

O comportamento do sistema pode ser ajustado alterando o número de **clientes**, **garçons** e **mesas**. Essas variáveis globais permitem configurar diferentes cenários de concorrência. Aumentar o número de clientes cria cenários mais concorridos, enquanto diminuir os garçons ou mesas aumenta a chance de espera e competição entre clientes.

## Tipos de Dados Usados

- **Threads**: Representam tanto **clientes** quanto **garçons**. As funções `pthread_create` e `pthread_join` são usadas para controlar a execução paralela.
- **Mutexes**: Usados para garantir que o acesso a cada mesa seja sincronizado, evitando condições de corrida.
- **Variáveis Condicionais**: Permitem que garçons aguardem até que uma mesa seja ocupada.

### Por que usar Mutexes?

- **Mutexes** foram usados em vez de **monitores** ou **troca de mensagens** porque:
  - **Monitores** em C exigem mais código para implementação manual, e **mutexes** já oferecem a granularidade necessária para o controle de acesso.
  - A **troca de mensagens** não seria ideal aqui, pois o problema envolve a sincronização de acesso a um recurso compartilhado (mesas), que é mais eficiente de gerenciar com mutexes.

## Garantias de Liveness e Evitar Deadlocks

- **Liveness**: As mesas são constantemente liberadas pelos garçons após atender um cliente, garantindo que o sistema continue progredindo e os clientes eventualmente consigam atendimento.
- **Deadlock**: O código evita deadlocks ao garantir que cada thread cliente ou garçom libere o mutex assim que uma mesa é liberada ou ocupada, evitando que dois threads fiquem esperando indefinidamente.
- **Condições de Corrida**: Usamos **mutexes** para proteger o acesso à variável `ocupado` de cada mesa, garantindo que um cliente não possa ocupá-la simultaneamente com outro.
- **Starvation**: Clientes repetem a busca por mesas até que uma fique disponível, evitando que algum cliente fique eternamente sem atendimento.

## Fluxo de Execução

1. **Clientes chegam e procuram mesas**: Cada cliente tenta sentar em uma mesa livre. Se uma mesa está ocupada, o cliente aguarda e tenta novamente.
2. **Garçons aguardam clientes**: Garçons esperam pela chegada de clientes. Assim que um cliente ocupa uma mesa, o garçom a atende.
3. **Mesas liberadas**: Após o atendimento, as mesas são liberadas e podem ser ocupadas por novos clientes.

## Requisitos para Rodar

1. **Compilador C** com suporte a **POSIX threads** (`pthread`).
2. **Makefile** para facilitar a compilação ou compile manualmente usando:
   ```bash
   gcc -pthread -o simulacao main.c
3. Sugiro rodar no replit.com
