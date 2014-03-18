La instalación de ces_import4ces crea las tablas necesarias en la base de 
datos para tener una relación de las importaciones realizadas.

Los campos que no sean recogidos en la importación seran guardados en el 
registro correspondiente en el campo data serializados.


tablas:

ces_import4ces_exchange (
      id,
      exchance_id,
      date,
      step,          ( Paso en el que nos hemos quedado )
      row,           ( última fila importada )
      uid,           ( Identificador de usuario que realiza la importación )
      data           ( campos que no recogemos serialzados )
      )

ces_import4ces_objects (
      id, 
      import_id,
      object,      ( Tipo de dato: usuario, transacción, etc.. )
      object_id,   ( Identificador de objeto )
      data         ( campos que no recogemos serialzados )
      )



El usuario administrativo del nuevo banco sera el que realiza la importación.


