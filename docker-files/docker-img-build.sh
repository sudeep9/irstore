
#!/bin/bash


img_name=ubuntu16:gcc7

docker build -t $img_name .
if [ $? -ne 0 ]; then
    echo "Error: docker build failed"
    exit 1
fi

docker tag $img_name img.druva.org/$img_name

echo "docker push img.druva.org/$img_name"