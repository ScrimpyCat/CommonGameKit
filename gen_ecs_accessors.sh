comp_ids_file=src/ecs/components/ECSComponentIDs.h

ecs_tool setup \
--input src/ecs/systems/*.h src/ecs/components/*.h \
--accessors src/ecs/systems/ECSSystemAccessors.h \
--config ecs_env \
--write components:ids \
$comp_ids_file

echo '#ifndef ECS_COMPONENT_ID_BASE' >> $comp_ids_file
echo '#define ECS_COMPONENT_ID_BASE 0' >> $comp_ids_file
echo '#endif' >> $comp_ids_file
echo '' >> $comp_ids_file
echo '#ifndef ECS_MONITOR_COMPONENT_ID_RELATIVE' >> $comp_ids_file
echo '#define ECS_MONITOR_COMPONENT_ID_RELATIVE 0' >> $comp_ids_file
echo '#endif' >> $comp_ids_file
echo '' >> $comp_ids_file
echo '#ifndef ECS_MONITOR_COMPONENT_ID' >> $comp_ids_file
echo '#define ECS_MONITOR_COMPONENT_ID (ECS_COMPONENT_ID_BASE + ECS_MONITOR_COMPONENT_ID_RELATIVE)' >> $comp_ids_file
echo '#endif' >> $comp_ids_file
