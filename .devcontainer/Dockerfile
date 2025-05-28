FROM debian:bookworm

# Initial setup
ENV USER=dockeruser
RUN apt-get update && apt-get install -y sudo && \
    useradd -m -s /bin/bash dockeruser && \
    echo "${USER} ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

# Install packages
RUN apt-get install --no-install-recommends -y \
    ca-certificates ssh wget \
    git \
    make \
    zsh

# Post install packages
RUN update-ca-certificates

# Install shell
USER dockeruser
RUN sh -c "$(wget https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh -O -)"
RUN git clone --depth=1 https://github.com/romkatv/powerlevel10k.git "${ZSH_CUSTOM:-$HOME/.oh-my-zsh/custom}/themes/powerlevel10k"
RUN git clone --depth=1 https://github.com/zsh-users/zsh-autosuggestions "${ZSH_CUSTOM:-$HOME/.oh-my-zsh/custom}/plugins/zsh-autosuggestions"
COPY --chown=${USER}:${USER} .zshrc /home/${USER}/.zshrc
COPY --chown=${USER}:${USER} .p10k.zsh /home/${USER}/.p10k.zsh
ENV SHELL=/bin/zsh

# Final setup
WORKDIR /workspace