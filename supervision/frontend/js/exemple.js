var listeMachine = $("<div>").addClass("divTypeMachine");

$(document).ready(function(){

    menuDeroulant.change(function ()
    {
        if($("option:selected",$(this)).data("jeu") === "zelda") {
            $("#zelda").empty();
            $("#zelda").append($("<p>").html("zelda est cool"));
        }
    });

    $(".button").click(function (){
        $("#zelda").append($("<p>").html("Yahaha, tu m'as trouvé !"));

        $.ajax({
            type: "GET",
            url: urlServeur,
            data: {"nomFonction" : "getTypeMachine"},
            success: 
            function(oRep){
                listeMachine.empty();

                for (let i = 0; i < oRep.length; i++) {
                    
                    listeMachine.append( $("<div>")
                                            .append( $("<h4>").html(oRep[i].nom )
                                            .attr("id", "type" + oRep[i].id) )
                                        )
                    ;
                }
            },
            error:
            function(){
                console.log("probleme");
            },
            dataType: "json"
        });
        
        $("#zelda").append(listeMachine);
    });

});