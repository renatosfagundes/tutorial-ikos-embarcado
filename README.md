
# Tutorial IKOS para Sistemas Embarcados Automotivos

## Visão Geral e Contexto Real

Este projeto tem como objetivo demonstrar o uso do IKOS (Inference Kernel for Open Static Analysis) para análise estática de código C em sistemas embarcados, com foco em aplicações automotivas. O trabalho foi realizado para fins acadêmicos, visando mostrar vulnerabilidades comuns e avaliar a viabilidade do IKOS no contexto automotivo.

**Importante:** O uso do IKOS apresentou diversas dificuldades práticas, limitações técnicas e problemas de integração, detalhados abaixo, que inviabilizam seu uso industrial na versão atual.

## Dificuldades e Limitações Encontradas

- **Ambiente Obrigatório:** A análise só foi possível em Ubuntu 22.04, pois outras versões apresentaram incompatibilidades.
- **Instalação via Snap:** O IKOS foi instalado exclusivamente via Snap, pois a compilação manual e o uso em containers Docker não funcionaram de forma confiável. O Snap não é suportado em containers Docker.
- **Containerização e CI/CD:** Apesar de ser teoricamente possível integrar o IKOS em pipelines CI/CD e containers, não foi possível entregar uma solução funcional em container no tempo disponível. O Snap não roda em Docker e a compilação manual do IKOS é complexa e sujeita a erros de dependência.
- **Limitações do ikos-view:** Não foi possível abrir o arquivo `output.db` no ikos-view, mesmo seguindo as instruções oficiais.
- **Relatórios:** A geração de relatórios HTML/JSON funcionou apenas parcialmente. A visualização web não foi plenamente integrada.
- **Cobertura:** O IKOS não cobre todos os padrões automotivos (ex: AUTOSAR, ISO 26262) e não substitui testes dinâmicos.
- **Recomendação:** O uso do IKOS **não é recomendado para aplicações industriais automotivas** na versão atual, servindo apenas para fins didáticos e experimentais.

## Forma de Uso Recomendada

1. Utilize obrigatoriamente o Ubuntu 22.04.
2. Instale o IKOS via Snap:
   ```bash
   sudo snap install ikos --edge --classic
   ```
3. Analise os arquivos de exemplo em `src-ikos-examples/` usando o comando:
   ```bash
   ikos <arquivo>.c
   ```
4. Gere relatórios (quando possível) com:
   ```bash
   ikos report -o output.html output.db
   ```
5. Tente abrir o relatório HTML no navegador. A abertura do `output.db` no ikos-view pode não funcionar.

## Exemplos de Código Utilizados

Os seguintes arquivos foram usados para testar o IKOS e gerar relatórios:

- buffer_overflow.c
- div_by_zero.c
- can_bus.c
- use_after_free.c
- uninit_var.c
- null_deref.c
- main.c
- double_free.c
- door_example.c

Cada arquivo contém vulnerabilidades típicas de sistemas embarcados e automotivos, como buffer overflow, uso de ponteiro nulo, uso após free, divisão por zero, entre outros.

## Observações Finais

- O IKOS é promissor para fins didáticos, mas **não está pronto para uso industrial automotivo**.
- A integração com CI/CD e containerização é limitada ou inviável na prática.
- O suporte a padrões automotivos é parcial.
- Recomenda-se o uso apenas para demonstração acadêmica e experimentação.

## Referências

- IKOS Official Documentation: https://github.com/NASA-SW-VnV/ikos
- MISRA C:2012 Guidelines: https://www.misra.org.uk
- ISO 26262 - Road Vehicles Functional Safety

- Docker Documentation: https://docs.docker.com
