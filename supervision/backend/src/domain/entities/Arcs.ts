import {
  Column,
  Entity,
  Index,
  JoinColumn,
  ManyToOne,
  PrimaryGeneratedColumn,
} from "typeorm";
import { Nodes } from "./Nodes";

@Index("destination_node_id", ["destinationNodeId"], {})
@Index("origin_node_id", ["originNodeId"], {})
@Entity("ARCS", { schema: "supervision" })
export class Arcs {
  @PrimaryGeneratedColumn({ type: "int", name: "id" })
  public id!: number;

  @Column("int", { name: "weight", nullable: true })
  public weight!: number | null;

  @Column("int", { name: "origin_node_id", nullable: true })
  public originNodeId!: number | null;

  @Column("int", { name: "destination_node_id", nullable: true })
  public destinationNodeId!: number | null;

  @Column("int", { name: "type", nullable: true })
  public type!: number | null;

  @ManyToOne(() => Nodes, (nodes) => nodes.arcs, {
    onDelete: "NO ACTION",
    onUpdate: "NO ACTION",
  })
  @JoinColumn([{ name: "origin_node_id", referencedColumnName: "id" }])
  public originNode!: Nodes;

  @ManyToOne(() => Nodes, (nodes) => nodes.arcs2, {
    onDelete: "NO ACTION",
    onUpdate: "NO ACTION",
  })
  @JoinColumn([{ name: "destination_node_id", referencedColumnName: "id" }])
  public destinationNode!: Nodes;

  public constructor(init?: Partial<Arcs>) {
    Object.assign(this, init);
  }
}
