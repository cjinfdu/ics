# ICS-Fall23-Fudan-Website

## Install

After pull this repo with git, you can cd into the directory and run this command to install hexo:

```sh
npm install --registry=http://registry.npmmirror.com
```

## How to use?

CAUTION: Please be sure to `git pull` before you do any change.

The `source` directory is the place to put **markdown files** and assets.
The index page corresponds to `source/index.md`.

`_config.yml` contains basic settings while `_config.cactus_white.yml` contains some theme-related settings.

### EXAMPLE: publish a new page

0. `git pull`
1. `hexo new <page_title>` to automatically generate a md file and an assets' dir in `source/_posts`
2. write your content and assets into the generated file/dir.
3. update `sourse/index.md` to add the link to your new page
3. `hexo g` to generate corresponding page
4. `hexo s` to start a server, which allows you to check the generated webpage.
5. `git pull` in case there are other changes.
6. `hexo d` to deploy the new site into the webpage repo.
7. `git add .` and commit your change. `git push` to sync.

