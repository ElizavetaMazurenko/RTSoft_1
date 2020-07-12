FROM alpine:latest
LABEL maintainer="Mazurenko Elizaveta"
LABEL description="This example Dockerfile installs NGINX and python3.5."
RUN apk add --update nginx && \
rm -rf /var/cache/apk/* && \
mkdir -p /tmp/nginx/ && \
apk add --update python3-dev
COPY ./README /etc/nginx/README
EXPOSE 80/tcp
ENTRYPOINT ["nginx"]
CMD ["-g", "daemon off;"] 
