var droptarget = document.getElementById("test");
droptarget.addEventListener("drop", function (event) {
  event.preventDefault(); //禁止浏览器默认行为，浏览器默认会直接打开显示
  if (event.type === "drop") {
    //获取文件信息，同File对象相同
    var files = event.dataTransfer.files;
    console.log(
      `文件名称:${files[0].name},文件类型:${files[0].type},文件大小:${files[0].size} bytes`
    );
  }
});
droptarget.addEventListener("dragenter", function (event) {
  //禁止浏览器默认行为，浏览器默认会直接打开显示
  event.preventDefault();
});
droptarget.addEventListener("dragover", function (event) {
  //禁止浏览器默认行为，浏览器默认会直接打开显示
  event.preventDefault();
});
