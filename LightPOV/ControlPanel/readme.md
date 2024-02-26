# LightPOV Control Panel

# Installations
+ Install Node.js
    - For Window Users, download and install Node.js from [here](https://nodejs.org/en/download/).
    - For Ubuntu Users, follow this [instruction](https://github.com/nodesource/distributions?tab=readme-ov-file#using-ubuntu-2):
        ```bash
        curl -fsSL https://deb.nodesource.com/setup_21.x | sudo -E bash - &&\
        sudo apt-get install -y nodejs
        ```
+ Install dependencies using `npm`:
    ```bash
    cd ES-Lux/LightPOV/ControlPanel
    npm install
    ```
    Or if you are a [`yarn`](https://classic.yarnpkg.com/en/) user, run `yarn install`.

# Quick Start
```bash
node server.js
```