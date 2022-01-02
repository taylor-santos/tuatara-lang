[![codecov](https://codecov.io/gh/taylor-santos/tuatara-lang/branch/trunk/graph/badge.svg?token=CA6VFEVDH6)](https://codecov.io/gh/taylor-santos/tuatara-lang)

<div style="text-align: center;">
<h2>tuatara-lang</h2>
<p>Compiler for the Tuatara programming language</p>
</div>

<details open="open">
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>

<!-- GETTING STARTED -->

## Getting Started

### Prerequisites

* CMake >= 3.16
* A C++20 supporting compiler
    * g++ >= 8
    * clang++ >= 9
    * MSVC >= 19.29
* Flex >= 2.6
* Bison >= 3.7

### Installation

1. Clone the repo and create a build directory:

    ```sh
    git clone https://github.com/taylor-santos/tuatara-lang
    cd tuatara-lang
    mkdir build
    cd build
    ```

2. Run CMake:
    ```sh
     cmake -D CMAKE_BUILD_TYPE=Release ..
    ```
3. Build
    ```sh
    cmake --build .
    ```
4. Run Application
    ```sh
    ./tuatara [options] file...
    ```
6. Run Tests
    ```sh
    cd test
    ctest -C Release --rerun-failed --output-on-failure
    ```

<!-- CONTRIBUTING -->

## Contributing

1. Fork the Project
1. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
1. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
1. Push to the Branch (`git push origin feature/AmazingFeature`)
1. Open a Pull Request

<!-- LICENSE -->

## License

Distributed under the MIT License. See `LICENSE` for more information.

<!-- CONTACT -->

## Contact

Taylor Santos - taylor.p.santos@gmail.com

Project Link: [https://github.com/taylor-santos/tuatara-lang](https://github.com/taylor-santos/tuatara-lang)
