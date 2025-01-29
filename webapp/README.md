# Data Logger

```ascii
 _____         __           _____                                
|     \.---.-.|  |_.---.-. |     |_.-----.-----.-----.-----.----.
|  --  |  _  ||   _|  _  | |       |  _  |  _  |  _  |  -__|   _|
|_____/|___._||____|___._| |_______|_____|___  |___  |_____|__|  
                                         |_____|_____|           
```

_This is where all the client front end resides._


*NOTE:* this project's package manager is *yarn*.  don't use npm.

*NOTE*: for linux development, use the latest node image that contains yarn: `docker run -it --name nodejs node:latest /bin/bash -l`

*TODO*: create a docker node develpment image for this project


## The Web App

## Customize configurations

See [Vite Configuration Reference](https://vite.dev/config/).

## Project Initialization

```sh
yarn install
```

### Compile and Hot-Reload for Development

```sh
yarn dev
```

### Compile and Minify for Production

```sh
yarn build
```

### Run Unit Tests with [Vitest](https://vitest.dev/)

```sh
yarn test:unit
```

### Run End-to-End Tests with [Cypress](https://www.cypress.io/)

```sh
yarn test:e2e:dev
```

This runs the end-to-end tests against the Vite development server.
It is much faster than the production build.

But it's still recommended to test the production build with `test:e2e` before deploying (e.g. in CI environments):

```sh
yarn build
yarn test:e2e
```

### Lint with [ESLint](https://eslint.org/)

```sh
yarn lint
```

## References

* [Vue Test Utils API](https://test-utils.vuejs.org/api/)

###### 2025.01.29 | dpw
