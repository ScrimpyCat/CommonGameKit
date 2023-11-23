comp_ids_file=src/ecs/components/ECSComponentIDs.h

ecs_tool setup \
--input src/ecs/systems/*.h src/ecs/components/*.h \
--accessors src/ecs/systems/ECSSystemAccessors.h \
--config ecs_env \
--write components:ids components:env \
$comp_ids_file
