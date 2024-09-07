FROM debian:bookworm-slim as builder
MAINTAINER projects@ve3lsr.ca

RUN apt-get update && apt-get install -y --no-install-recommends \
            git \
            build-essential \
            libcurl4-openssl-dev \
            libboost-dev \
            libgps-dev \
            ca-certificates \
         && apt-get autoremove -y && apt-get clean -y && rm -rf /var/lib/apt/lists/*

WORKDIR /opt/DStarGateway

ADD ./.git /opt/DStarGateway/.git
ADD ./APRS /opt/DStarGateway/APRS
ADD ./BaseCommon /opt/DStarGateway/BaseCommon
ADD ./Common /opt/DStarGateway/Common
ADD ./DGWRemoteControl /opt/DStarGateway/DGWRemoteControl
ADD ./DGWTextTransmit /opt/DStarGateway/DGWTextTransmit
ADD ./DGWTimeServer /opt/DStarGateway/DGWTimeServer
ADD ./DGWVoiceTransmit /opt/DStarGateway/DGWVoiceTransmit
ADD ./DStarBase /opt/DStarGateway/DStarBase
ADD ./DStarGateway /opt/DStarGateway/DStarGateway
ADD ./IRCDDB /opt/DStarGateway/IRCDDB
ADD ./VersionInfo /opt/DStarGateway/VersionInfo
ADD ./Makefile /opt/DStarGateway/

RUN ls -l

RUN make USE_GPS=1 #ENABLE_DEBUG=1

FROM debian:bookworm-slim
MAINTAINER projects@ve3lsr.ca

RUN apt-get update && apt-get install -y --no-install-recommends \
            wget \
            libcurl4 \
            cron \
         && apt-get autoremove -y && apt-get clean -y && rm -rf /var/lib/apt/lists/*

RUN groupadd -g 999 dstar
RUN useradd -m -u 999 -g 999 -s /bin/none dstar

## Add the DStarGateway Binary Files
ADD ./Data /usr/local/share/dstargateway.d/
COPY --from=builder /opt/DStarGateway/DStarGateway/dstargateway /usr/local/bin/
COPY --from=builder /opt/DStarGateway/DGWRemoteControl/dgwremotecontrol /usr/local/bin/
COPY --from=builder /opt/DStarGateway/DGWTextTransmit/dgwtexttransmit /usr/local/bin/
COPY --from=builder /opt/DStarGateway/DGWTimeServer/dgwtimeserver /usr/local/bin/
COPY --from=builder /opt/DStarGateway/DGWVoiceTransmit/dgwvoicetransmit /usr/local/bin/
ADD Docker/start-dstargateway.sh /
ADD Docker/start-timeserver.sh /

# Default Entrypoint is DStarGateway
ENTRYPOINT /start-dstargateway.sh
