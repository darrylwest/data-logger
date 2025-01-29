# WebApp Starter

_This is where all the client front end resides.

*NOTE*: for linux development, use the latest node image that contains yarn: `docker run -it --name nodejs node:latest /bin/bash -l`

*TODO*: create a node develpment image for this project


## The Starter Web App

Simple starter with minimal header, footer, Landing.vue, Home.vue, Login.vue, Home.vue, About.vue, and other pages

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

###### 2025.01.10 | dpw
