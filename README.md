# TP02_SO_Sistema_de_Arquivos

## Compilar e executar o programa

### Compilar e executar com tamanho de bloco padrão e tamanho de partição padrão
```{bash}
make run
```

### Compilar e executar escolhendo o tamanho de bloco e tamanho de partição
Para definir o tamanho do bloco e o tamanho da partição, defina BLOCK_SIZE_KB=\<valor\>
e PARTITION_SIZE_MB=\<valor\> como no exemplo abaixo(os valores são em KB e MB, respectivamente,
e o valor de BLOCK_SIZE_KB deve ser de 1 a 4 (inclusivo), e de PARTITION_SIZE_MB de 10 a 100 (inclusivo)):
```{bash}
make run BLOCK_SIZE_KB=1 PARTITION_SIZE_MB=50
```
