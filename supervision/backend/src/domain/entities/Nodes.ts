import {
  Column,
  Entity,
  Index,
  JoinColumn,
  ManyToOne,
  OneToMany,
  PrimaryGeneratedColumn,
} from "typeorm";
import { Arcs } from "./Arcs";
import { NodesTypes } from "./NodesTypes";
import { TravelsNodes } from "./TravelsNodes";

@Index("node_type_id", ["nodeTypeId"], {})
@Entity("NODES", { schema: "supervision" })
export class Nodes {
  @PrimaryGeneratedColumn({ type: "int", name: "id" })
  public id!: number;

  @Column("varchar", { name: "name", nullable: true, length: 255 })
  public name!: string | null;

  @Column("float", { name: "position_x", nullable: true, precision: 12 })
  public positionX!: number | null;

  @Column("float", { name: "position_y", nullable: true, precision: 12 })
  public positionY!: number | null;

  @Column("tinyint", { name: "point_of_interest", nullable: true, width: 1 })
  public pointOfInterest!: boolean | null;

  @Column("float", { name: "offset_x", nullable: true, precision: 12 })
  public offsetX!: number | null;

  @Column("float", { name: "offset_y", nullable: true, precision: 12 })
  public offsetY!: number | null;

  @Column("int", { name: "node_type_id", nullable: true })
  public nodeTypeId!: number | null;

  @OneToMany(() => Arcs, (arcs) => arcs.originNode)
  public arcs!: Arcs[];

  @OneToMany(() => Arcs, (arcs) => arcs.destinationNode)
  public arcs2!: Arcs[];

  @ManyToOne(() => NodesTypes, (nodesTypes) => nodesTypes.nodes, {
    onDelete: "NO ACTION",
    onUpdate: "NO ACTION",
  })
  @JoinColumn([{ name: "node_type_id", referencedColumnName: "id" }])
  public nodeType!: NodesTypes;

  @OneToMany(() => TravelsNodes, (travelsNodes) => travelsNodes.node)
  public travelsNodes!: TravelsNodes[];

  public constructor(init?: Partial<Nodes>) {
    Object.assign(this, init);
  }
}
