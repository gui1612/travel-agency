# FEUP-DA-PROJ2

## Second Project for the DA course unit at FEUP.

The main focus of this project is to implement a system capable of managing travel groups, using graphs to compute the best routes.

## Building

This project uses [cmake](https://cmake.org/). Assuming it's installed and properly setup, simply run:

1. `cmake -S . -B build` in this project's directory to generate the build system.
2. To build the project's documentation, run `cmake --build build --target documentacao`.
3. To build the project's executable, run `cmake --build build --config Release --target travel`.
   - The executable can be found under the `build/codigo/` directory, with the name `travel`.

**Warning:** Execute these commands on the root of the repository (where the codigo, documentacao, input and apresentacao folders are located at)! 

## Running

Simply run the executable generated in the last section.

### Not sorting the results

By default, the flow results are sorted, but, if you don't want them sorted, you can use the `--no-sort` option.

Example: `travel --no-sort`

## Unit info

* **Name**: Desenho de Algoritmos (Algorithm Design)
* **Date**: Year 2, Semester 2, 2021/22
* [**More info**](https://sigarra.up.pt/feup/pt/ucurr_geral.ficha_uc_view?pv_ocorrencia_id=484424)

## Disclaimer

This repository is for archival and educational purposes only.

If you don't understand some part of the code or anything else in this repo, feel free to ask (although we may not understand it anymore).

Keep in mind that this repo is public. If you copy any code and use it in your school projects you may be flagged for plagiarism by automated tools.